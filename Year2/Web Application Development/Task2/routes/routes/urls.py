
app_name = "routes"

from django.urls import path

from . import views


urlpatterns = [
    path("", views.index, name="index"),
    path("login/", views.login, name="login"),
    path("logout/", views.logout, name="logout"),
    path("register/", views.register, name="register"),
    path("my-routes/", views.my_routes, name="my_routes"),
    path("route/<int:pk>/", views.route_detail, name="route_detail"),
    path("new-route/", views.route_detail, name="new_route"),
    path("route/<int:pk>/delete/", views.route_delete, name="route_delete"),
    path("point/<int:pk>/delete/", views.point_delete, name="point_delete"),
    path("gameboard/", views.gameboard, name="gameboard"),
    path("gameboard_save/", views.gameboard_save, name="gameboard_save"),
    path("my_gameboards/", views.my_gameboards, name="my_gameboards"),
    path("gameboard/<int:pk>/", views.gameboard, name="gameboard"),
    path("gameboard/<int:pk>/delete/", views.gameboard_delete, name="gameboard_delete"),
    path("my_paths/", views.my_paths, name="my_paths"),
    path("path_entry/<int:pk>/", views.path_entry, name="path_entry"),
    path("path/<int:pk>/", views.path, name="path"),
    path("path_save/<int:pk>/", views.path_save, name="path_save"),
    path("sse/notifications/", views.sse_notifications, name="sse_notifications")
]