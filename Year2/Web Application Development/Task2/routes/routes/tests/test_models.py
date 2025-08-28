from django.test import TestCase
from django.contrib.auth import get_user_model
from routes.models import Route, RoutePoint, BackgroundImage
from django.core.exceptions import ValidationError
from django.db import IntegrityError

class ModelTests(TestCase):
    def setUp(self):
        self.user = get_user_model().objects.create_user(username='testuser', password='testpass')
        self.image = BackgroundImage.objects.create(name='Test Image', image='test.jpg')
        self.route = Route.objects.create(name='Test Route', user=self.user, background=self.image)
        self.point1 = RoutePoint.objects.create(route=self.route, x=69, y=42.0)
        self.point2 = RoutePoint.objects.create(route=self.route, x=42.0, y=69, order=1)

    def test_create_user(self):
        self.assertEqual(self.user.username, 'testuser')

    def test_create_background(self):
        self.assertEqual(self.image.name, 'Test Image')
        self.assertEqual(self.image.image, 'test.jpg')

    def test_create_route(self):
        self.assertEqual(self.route.name, 'Test Route')
        self.assertEqual(self.route.user, self.user)
        self.assertEqual(self.route.background, self.image)

    def test_create_route_point(self):
        self.assertEqual(self.point1.route, self.route)
        self.assertEqual(self.point1.x, 69)
        self.assertEqual(self.point1.y, 42)
        self.assertEqual(self.point1.order, 0)

        self.assertEqual(self.point2.route, self.route)
        self.assertEqual(self.point2.x, 42)
        self.assertEqual(self.point2.y, 69.0)
        self.assertEqual(self.point2.order, 1)

    def test_route_relationship(self):
        self.assertEqual(self.route.user, self.user)
        self.assertEqual(self.route.background, self.image)

        self.assertIn(self.route, self.user.routes.all())
        self.assertIn(self.route, self.image.routes.all())

    def test_route_point_relationship(self):
        self.assertEqual(self.point1.route, self.route)
        self.assertEqual(self.point2.route, self.route)

        self.assertIn(self.point1, self.route.points.all())
        self.assertIn(self.point2, self.route.points.all())

    def test_route_point_ordering(self):
        with self.assertRaises(IntegrityError):
            RoutePoint.objects.create(route=self.route, x=69, y=42, order=0)

    def test_route_point_invalid_x(self):
        invalid_point = RoutePoint.objects.create(route=self.route, x=-1, y=42, order=2)
        with self.assertRaises(ValidationError):
            invalid_point.full_clean()

    def test_route_point_invalid_y(self):
        invalid_point = RoutePoint.objects.create(route=self.route, x=69, y=101, order=3)
        with self.assertRaises(ValidationError):
            invalid_point.full_clean()

    def test_route_point_invalid_route(self):
        with self.assertRaises(IntegrityError):
            RoutePoint.objects.create(route=None, x=69, y=42, order=2)

    def test_route_invalid_background(self):
        with self.assertRaises(IntegrityError):
            Route.objects.create(name='Invalid Route', user=self.user, background=None)
