from rest_framework import serializers
from dj_rest_auth.registration.serializers import RegisterSerializer
from drf_yasg.utils import swagger_serializer_method
from django.contrib.auth import get_user_model

from .models import Route, RoutePoint, BackgroundImage

class RoutePointSerializer(serializers.ModelSerializer):
    class Meta:
        model = RoutePoint
        fields = ['id', 'x', 'y', 'order']

    def validate_x(self, value):
        if not (0 <= value <= 100):
            raise serializers.ValidationError("X coordinate must be between 0 and 100.")
        return value

    def validate_y(self, value):
        if not (0 <= value <= 100):
            raise serializers.ValidationError("Y coordinate must be between 0 and 100.")
        return value
    
    def validate_order(self, value):
        if value < 0:
            raise serializers.ValidationError("Order must be zero or a positive integer.")
        return value


class RouteSerializer(serializers.ModelSerializer):
    points = RoutePointSerializer(many=True, read_only=True)
    background = serializers.PrimaryKeyRelatedField(queryset=BackgroundImage.objects.all(), write_only=True)
    background_image = serializers.ImageField(source='background.image', read_only=True)

    class Meta:
        model = Route
        fields = ['id', 'name', 'background', 'background_image', 'created_at', 'points']



class CustomRegisterSerializer(RegisterSerializer):
    class Meta:
        fields = ['username', 'password']
        

    email = None
    password1 = None
    password2 = None

    password = serializers.CharField(write_only=True, required=True)

    def validate(self, data):
        if 'password' in data:
            password = data.get('password')
            if not password:
                raise serializers.ValidationError("Password is required.")

        return data

    def save(self, request):
        user = get_user_model().objects.create_user(
            username=self.validated_data['username'],
            password=self.validated_data['password'],
        )
        return user
    
    @swagger_serializer_method(serializer_or_field=serializers.CharField)
    def get_swagger_fields(cls):
        return {
            'username': serializers.CharField(required=True),
            'password': serializers.CharField(write_only=True, required=True)
        }