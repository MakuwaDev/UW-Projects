from django.db import models
from django.contrib.auth.models import User
from django.core.exceptions import ValidationError
from django.contrib.postgres.fields import JSONField

class BackgroundImage(models.Model):
    name = models.CharField(max_length=100, null=False, blank=False)
    image = models.ImageField(upload_to="backgrounds/", null=False, blank=False)

    class Meta:
        verbose_name = "Background Image"
        verbose_name_plural = "Background Images"

    def __str__(self):
        return self.name
    

class Route(models.Model):
    name = models.CharField(max_length=100, null=False, blank=False)
    user = models.ForeignKey(User, on_delete=models.CASCADE, related_name="routes", null=False)
    background = models.ForeignKey(BackgroundImage, on_delete=models.CASCADE, related_name="routes", null=False)
    created_at = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return f'{self.name} (by {self.user.username})'
    

class RoutePoint(models.Model):
    route = models.ForeignKey(Route, on_delete=models.CASCADE, related_name="points", null=False)
    x = models.FloatField(null=False, blank=False)
    y = models.FloatField(null=False, blank=False)
    order = models.IntegerField(default=0, null=False, blank=False)

    class Meta:
        verbose_name = "Route Point"
        verbose_name_plural = "Route Points"
        ordering = ['order']
        unique_together = ('route', 'order')

    def clean(self):
        if not (0 <= self.x <= 100):
            raise ValidationError("X coordinate must be between 0 and 100.")
        
        if not (0 <= self.y <= 100):
            raise ValidationError("Y coordinate must be between 0 and 100.")

    def __str__(self):
        return f'Point {self.order} of {self.route.name}: ({self.x}, {self.y})'
    

class GameBoard(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE, related_name="game_boards", null=False)
    title = models.CharField(max_length=100,  null=False, blank=False)
    rows = models.IntegerField(null=False, blank=False)
    cols = models.IntegerField(null=False, blank=False)
    created_at = models.DateTimeField(auto_now_add=True)
    dots = models.JSONField()

    def __str__(self):
        return f'{self.title} (by {self.user.username})'
    

class Path(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE, null=False)
    board = models.ForeignKey(GameBoard, on_delete=models.CASCADE, null=False)
    created_at = models.DateTimeField(auto_now_add=True)
    path = models.JSONField()