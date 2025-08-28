from rest_framework.routers import DefaultRouter
from rest_framework.authentication import TokenAuthentication
from rest_framework import permissions
from rest_framework_nested import routers
from django.urls import path, include
from dj_rest_auth.views import LoginView, LogoutView
from drf_yasg.views import get_schema_view
from drf_yasg import openapi

from .views import RouteViewSet, RoutePointViewSet, CustomRegisterView

router = DefaultRouter()
router.register('routes', RouteViewSet, basename='route')

routes_router = routers.DefaultRouter()
routes_router.register('routes', RouteViewSet, basename='route')


points_router = routers.NestedDefaultRouter(routes_router, 'routes', lookup='route')
points_router.register('points', RoutePointViewSet, basename='route-points')

schema_view = get_schema_view(
    openapi.Info(
        title='Route API',
        default_version='v1',
        description='API for managing routes and points.',
    ),
    public=True,
    permission_classes=(permissions.AllowAny,),
    authentication_classes=[TokenAuthentication]
)

urlpatterns = [
    path('', include(routes_router.urls)),
    path('', include(points_router.urls)),
    path('auth/login/', LoginView.as_view(), name='auth_login'),
    path('auth/logout/', LogoutView.as_view(), name='auth_logout'),
    path('auth/register/', CustomRegisterView.as_view(), name='auth_register'),

    path('swagger/', schema_view.with_ui('swagger', cache_timeout=0), name='schema-swagger-ui'),

    path('redoc/', schema_view.with_ui('redoc', cache_timeout=0), name='schema-redoc'),
]