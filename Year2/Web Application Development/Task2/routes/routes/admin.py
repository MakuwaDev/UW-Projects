from django.contrib import admin
from django.contrib.sites.models import Site
from django.contrib.auth.models import Group
from rest_framework.authtoken.models import Token
from allauth.account.models import EmailAddress
from allauth.socialaccount.models import SocialAccount, SocialApp, SocialToken

from .models import BackgroundImage, Route, RoutePoint, GameBoard, Path

admin.site.unregister(SocialAccount)
admin.site.unregister(SocialApp)
admin.site.unregister(SocialToken)
admin.site.unregister(EmailAddress)
admin.site.unregister(Site)
admin.site.unregister(Group)

@admin.register(BackgroundImage)
class BackgroundImageAdmin(admin.ModelAdmin):
    list_display = ('name', 'image', 'pk')


@admin.register(Route)
class RouteAdmin(admin.ModelAdmin):
    list_display = ('name', 'user', 'background', 'created_at', 'pk')
    search_fields = ('name', 'user__username', 'pk')

    def custom_method(self, obj):
        return f"Custom Value for {obj.name}"


@admin.register(RoutePoint)
class RoutePointAdmin(admin.ModelAdmin):
    list_display = ('route', 'x', 'y', 'order', 'pk')
    search_fields = ('route__name','pk')

@admin.register(GameBoard)
class GameBoardAdmin(admin.ModelAdmin):
    list_display = ('title', 'user', 'rows', 'cols', 'created_at', 'dots', 'pk')
    search_fields = ('title', 'user__username', 'pk')

@admin.register(Path)
class PathAdmin(admin.ModelAdmin):
    list_display = ('board', 'user', 'pk')
    search_fields = ('board', 'user', 'pk')