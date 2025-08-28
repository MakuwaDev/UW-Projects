from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth import login as auth_login, authenticate, logout as auth_logout
from django.contrib.auth.forms import UserCreationForm, AuthenticationForm
from django.contrib.auth.decorators import login_required
from django.views.decorators.csrf import csrf_exempt
from django.db.models import Max
from rest_framework import viewsets
from rest_framework.permissions import IsAuthenticated
from rest_framework.exceptions import ValidationError
from rest_framework.response import Response
from rest_framework import status, permissions
from dj_rest_auth.registration.views import RegisterView
from drf_yasg.utils import swagger_auto_schema
import json
from django.http import JsonResponse, StreamingHttpResponse

from .models import Route, RoutePoint, GameBoard, Path
from .forms import RouteForm, RoutePointForm
from .serializers import RouteSerializer, RoutePointSerializer

@login_required
def index(request):
    return render(request, "index.html", {"user": request.user})


def register(request):
    if request.method == "POST":
        form = UserCreationForm(request.POST)

        if form.is_valid():
            user = form.save()
            auth_login(request, user)
            return redirect("routes:index")
    else:
        form = UserCreationForm()
        
    return render(request, "auth/register.html", {"form": form})


def login(request):
    if request.method == "POST":
        form = AuthenticationForm(data=request.POST)

        if form.is_valid():
            username = form.cleaned_data.get("username")
            password = form.cleaned_data.get("password")
            user = authenticate(username=username, password=password)

            if user is not None:
                auth_login(request, user)
                return redirect("routes:index")
            else:
                form.add_error(None, "Invalid username or password")
    else:
        form = AuthenticationForm()

    return render(request, "auth/login.html", {"form": form})


def logout(request):
    auth_logout(request)
    return redirect("routes:login")


@login_required
def my_routes(request):
    routes = Route.objects.filter(user=request.user)
    return render(request, "routes/my_routes.html", {"routes": routes})


@login_required
def route_detail(request, pk=None):
    route = None
    route_form = RouteForm()
    point_form = RoutePointForm()

    if pk:
        route = get_object_or_404(Route, pk=pk)

        if route.user != request.user:
            return redirect("routes:my_routes")

    if request.method == "POST":
        if "save_route" in request.POST:
            route_form = RouteForm(request.POST, instance=route)

            if route_form.is_valid():
                new_route = route_form.save(commit=False)
                new_route.user = request.user
                new_route.background = route_form.cleaned_data.get("background")
                new_route.save()
                return redirect("routes:route_detail", pk=new_route.pk)
        elif "add_point" in request.POST:
            point_form = RoutePointForm(request.POST)

            if point_form.is_valid():
                point = point_form.save(commit=False)
                point.route = route
                if route.points.exists():
                    max_order = route.points.aggregate(Max('order'))['order__max']
                    point.order = max_order + 1
                point.save()
                return redirect("routes:route_detail", pk=route.pk)


    context = {
        "route_form": route_form,
        "route": route,
        "point_form": point_form,
    }

    return render(request, "routes/route_detail.html", context)


@login_required
def route_delete(request, pk):
    route = get_object_or_404(Route, pk=pk)
    
    if route.user != request.user:
        return redirect("routes:my_routes")
    
    if request.method == "POST":
        route.delete()
        return redirect("routes:my_routes")
    
    return render(request, "routes/route_delete.html", {"route": route})


@login_required
def point_delete(request, pk):
    point = get_object_or_404(RoutePoint, pk=pk)

    if point.route.user != request.user:
        return redirect("routes:my_routes")
    
    if request.method == "POST":
        point.delete()
        return redirect("routes:route_detail", pk=point.route.pk)
    
    return render(request, "routes/point_delete.html", {"point": point})


class RouteViewSet(viewsets.ModelViewSet):
    serializer_class = RouteSerializer
    permission_classes = [IsAuthenticated]

    def get_queryset(self):
        user = self.request.user
        if not user.is_authenticated:
            return Route.objects.none()
        return Route.objects.filter(user=self.request.user)
    
    def perform_create(self, serializer):
        serializer.save(user=self.request.user)

    def destroy(self, request, *args, **kwargs):
        route = self.get_object()
        if route.user != request.user:
            return Response({'detail': "You are not authorized to delete this route."}, status=status.HTTP_403_FORBIDDEN)
        return super().destroy(request, *args, **kwargs)


class RoutePointViewSet(viewsets.ModelViewSet):
    serializer_class = RoutePointSerializer
    permission_classes = [IsAuthenticated]

    def get_queryset(self):
        if getattr(self, 'swagger_fake_view', False):
            return RoutePoint.objects.none()

        route_id = self.kwargs['route_pk']
        return RoutePoint.objects.filter(route__id=route_id)
    
    def perform_create(self, serializer):
        route = Route.objects.get(pk=self.kwargs['route_pk'])

        x = serializer.validated_data.get('x')
        y = serializer.validated_data.get('y')

        if RoutePoint.objects.filter(route=route, x=x, y=y).exists():
            raise ValidationError("Point with these coordinates already exists on this route.")

        last_point = RoutePoint.objects.filter(route=route).order_by('-order').first()
        new_order = last_point.order + 1 if last_point else 0

        serializer.save(route=route, order=new_order)

        return Response(route.points.all(), status=status.HTTP_201_CREATED)
    
    def destroy(self, request, *args, **kwargs):
        point = self.get_object()
        if point.route.user != request.user:
            return Response({'detail': "You are not authorized to delete this point."}, status=status.HTTP_403_FORBIDDEN)
        return super().destroy(request, *args, **kwargs)
    

class CustomRegisterView(RegisterView):
    permission_classes = (permissions.AllowAny,)
    @swagger_auto_schema(
        operation_description="Registers a new user with the following fields: username, password"
    )
    def post(self, request, *args, **kwargs):
        return super().post(request, *args, **kwargs)


@login_required
def gameboard(request, pk=None):
    if pk:
        gameboard = get_object_or_404(GameBoard, pk=pk, user=request.user)
        return render(request, "gameboards/gameboard_edit.html", {
            "user": request.user,
            "gameboard": gameboard,
        })
    else:
        return render(request, "gameboards/gameboard_edit.html", {"user": request.user})


@login_required
def my_gameboards(request):
    gameboards = GameBoard.objects.filter(user=request.user)
    return render(request, "gameboards/my_gameboards.html", {"gameboards": gameboards})


@login_required
def gameboard_save(request):
    if request.method != "POST":
        return JsonResponse({"error": "Invalid method."}, status=405)

    try:
        data = json.loads(request.body)
    except json.JSONDecodeError:
        return JsonResponse({"error": "Invalid JSON."}, status=400)
    
    title = data.get("title", "")
    rows = data.get("rows")
    cols = data.get("cols")
    dots = data.get("dots")
    board_pk = data.get("pk")

    errors = {}

    if not title:
        errors["title"] = "Title is required."

    if not isinstance(rows, int) or rows <= 0:
        errors["rows"] = "Row count must be a positive integer."

    if not isinstance(cols, int) or cols <= 0:
        errors["cols"] = "Column count must be a positive integer."

    if not isinstance(dots, list):
        errors["dots"] = "Dots must be a list."
    else:
        for i, dot in enumerate(dots):
            if not isinstance(dot, dict):
                errors[f"dots[{i}]"] = "Each dot must be an object."
                continue

            try:
                x = int(dot.get("col"))
                y = int(dot.get("row"))
            except (TypeError, ValueError):
                errors[f"dots[{i}]"] = "x and y must be integers."
                continue

            if not (0 <= x < cols) or not (0 <= y < rows):
                errors[f"dots[{i}]"] = f"x must be in [0, {cols-1}], y must be in [0, {rows-1}]."
    
    if errors:
        return JsonResponse({"success": False, "errors": errors}, status=400)
    
    if board_pk:
        try:
            board = GameBoard.objects.get(pk=board_pk)
        except GameBoard.DoesNotExist:
            return JsonResponse({"error": "Board not found."}, status=404)
        
        if board.user != request.user:
            return JsonResponse({"error": "Unauthorized."}, status=403)
    else:
        board = GameBoard(user=request.user)

    board.title = title
    board.rows = rows
    board.cols = cols
    board.dots = json.dumps(dots)
    board.save()

    related_paths = Path.objects.filter(board=board)

    for path in related_paths:
        path.delete()

    return JsonResponse({"success": True, "id": board.pk})


@login_required
def gameboard_delete(request, pk):
    board = get_object_or_404(GameBoard, pk=pk)
    
    if board.user != request.user:
        return redirect("routes:my_gameboards")
    
    if request.method == "POST":
        board.delete()
        return redirect("routes:my_gameboards")
    
    return render(request, "gameboards/gameboard_delete.html", {"board": board})


@login_required
def my_paths(request):
    gameboards = GameBoard.objects.all()
    return render(request, "paths/my_paths.html", {"gameboards": gameboards})


@login_required
def path_entry(request, pk):
    board = get_object_or_404(GameBoard, pk=pk)

    path_obj, created = Path.objects.get_or_create(
        user=request.user,
        board=board,
        defaults={'path': []}
    )

    return redirect("routes:path", pk=path_obj.pk)


@login_required
def path(request, pk):
    path_obj = get_object_or_404(Path, pk=pk)

    if path_obj.user != request.user:
        return redirect("routes:index")
    
    return render(request, "paths/path_edit.html", {
        "board": path_obj.board,
        "path": path_obj,
    })


@login_required
def path_save(request, pk):
    print("saving")
    if request.method != "POST":
        return JsonResponse({"error": "Invalid method."}, status=405)

    try:
        paths = json.loads(request.body)
    except json.JSONDecodeError:
        return JsonResponse({"error": "Invalid JSON."}, status=400)
    
    path = Path.objects.get(pk=pk)
    if path.user != request.user:
        return JsonResponse({"error": "Wrong user."}, status=403)
    
    board = path.board

    errors = {}

    if not isinstance(paths, list):
        errors["paths"] = "Paths must be a list."
    else:
        for i, cell in enumerate(paths):
            if not isinstance(cell, dict):
                errors[f"paths[{i}]"] = "Each cell must be an object."
                continue

            try:
                x = int(cell.get("col"))
                y = int(cell.get("row"))
            except (TypeError, ValueError):
                errors[f"paths[{i}]"] = "x and y must be integers."
                continue

            if not (0 <= x < board.cols) or not (0 <= y < board.rows):
                errors[f"dots[{i}]"] = f"x must be in [0, {board.cols-1}], y must be in [0, {board.rows-1}]."
    
    if errors:
        return JsonResponse({"success": False, "errors": errors}, status=400)
    
    path.path = json.dumps(paths)
    path.save()

    return JsonResponse({"success": True, "id": board.pk})


events = []


def event_stream(start_id):
    timer = 0
    last_sent_id = start_id
    while True:
        timer += 1
        if timer > 1000000:
            yield ": keep alive\n\n"
            timer = 0

        while (len(events) > last_sent_id):
            event = events[last_sent_id]
            last_sent_id += 1
            yield f"event: {event['type']}\n"
            yield f"data: {json.dumps(event['data'])}\n\n"


current_id = [0]


def sse_notifications(request):
    response = StreamingHttpResponse(event_stream(current_id[0]), content_type='text/event-stream')
    response['Cach-Control'] = 'no-cache'
    return response