from rest_framework.test import APITestCase, APIClient
from rest_framework.authtoken.models import Token
from django.contrib.auth import get_user_model
from django.urls import reverse
from routes.models import Route, RoutePoint, BackgroundImage

class AuthTests(APITestCase):
    def setUp(self):
        self.user1 = get_user_model().objects.create_user(username='testuser1', password='testpass')
        self.user2 = get_user_model().objects.create_user(username='testuser2', password='testpass')

        self.token1 = Token.objects.create(user=self.user1)
        self.token2 = Token.objects.create(user=self.user2)

        self.client.credentials(HTTP_AUTHORIZATION='Token ' + self.token1.key)

    def test_requires_authentication(self):
        self.client.credentials()
        response = self.client.get('/api/routes/')
        self.assertEqual(response.status_code, 401)

    def test_only_own_data_visible(self):
        image = BackgroundImage.objects.create(name='Test Image', image='test.jpg')
        Route.objects.create(name='user1 route', user=self.user1, background=image)
        Route.objects.create(name='user2 route', user=self.user2, background=image)

        response = self.client.get('/api/routes/')

        self.assertEqual(len(response.json()), 1)
        self.assertEqual(response.json()[0]['name'], 'user1 route')


class RouteApiTests(APITestCase):
    def setUp(self):
        self.user = get_user_model().objects.create_user(username='testuser', password='testpass')
        self.token = Token.objects.create(user=self.user)
        self.client.credentials(HTTP_AUTHORIZATION='Token ' + self.token.key)

        self.image = BackgroundImage.objects.create(name='Test Image', image='test.jpg')
        self.route = Route.objects.create(name='Test Route', user=self.user, background = self.image)
        self.point = RoutePoint.objects.create(route=self.route, x=69, y=42.0)

    def test_create_route(self):
        response = self.client.post('/api/routes/', {
            'name': 'New Route',
            'background': self.image.id,
        })

        self.assertEqual(response.status_code, 201)
        self.assertEqual(response.data['name'], 'New Route')

    def test_route_detail(self):
        response = self.client.get(f'/api/routes/{self.route.id}/')
        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.data['name'], 'Test Route')
        self.assertEqual(response.data['points'][0]['x'], 69.0)
        self.assertEqual(response.data['points'][0]['y'], 42)

    def test_delete_route(self):
        response = self.client.delete(f'/api/routes/{self.route.id}/')
        self.assertEqual(response.status_code, 204)
        self.assertFalse(Route.objects.filter(id=self.route.id).exists())

    def test_change_route(self):
        new_image = BackgroundImage.objects.create(name='New Image', image='new.jpg')

        response = self.client.put(f'/api/routes/{self.route.id}/',{
            'name': 'Updated Route',
            'background': new_image.id,
        })

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.data['name'], 'Updated Route')
        self.assertEqual(response.data['points'][0]['x'], 69.0)
        self.assertEqual(response.data['points'][0]['y'], 42)

        self.assertTrue(Route.objects.filter(id=self.route.id, name='Updated Route').exists())
        self.assertFalse(Route.objects.filter(id=self.route.id, name='Test Route').exists())


class RoutePointApiTests(APITestCase):
    def setUp(self):
        self.user = get_user_model().objects.create_user(username='testuser', password='testpass')
        self.token = Token.objects.create(user=self.user)
        self.client.credentials(HTTP_AUTHORIZATION='Token ' + self.token.key)

        self.image = BackgroundImage.objects.create(name='Test Image', image='test.jpg')
        self.route = Route.objects.create(name='Test Route', user=self.user, background=self.image)
        self.point = RoutePoint.objects.create(route=self.route, x=69, y=42.0)

    def test_add_point(self):
        response = self.client.post(f'/api/routes/{self.route.id}/points/', {
            'x': 10,
            'y': 20,
            'order': 1
        })

        self.assertEqual(response.status_code, 201)
        self.assertEqual(response.data['x'], 10)
        self.assertEqual(response.data['y'], 20)
        self.assertEqual(response.data['order'], 1)

    def test_list_points(self):
        response = self.client.get(f'/api/routes/{self.route.id}/points/')
        self.assertEqual(response.status_code, 200)
        self.assertEqual(len(response.data), 1)
        self.assertEqual(response.data[0]['x'], 69.0)
        self.assertEqual(response.data[0]['y'], 42)
        self.assertEqual(response.data[0]['order'], 0)

    def test_delete_point(self):
        response = self.client.delete(f'/api/routes/{self.route.id}/points/{self.point.id}/')

        self.assertEqual(response.status_code, 204)
        self.assertFalse(RoutePoint.objects.filter(id=self.point.id).exists())

    def test_change_point(self):
        response = self.client.put(f'/api/routes/{self.route.id}/points/{self.point.id}/', {
            'x': 100,
            'y': 1,
            'order': 1
        })

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.data['x'], 100)
        self.assertEqual(response.data['y'], 1)
        self.assertEqual(response.data['order'], 1)

        self.assertTrue(RoutePoint.objects.filter(id=self.point.id, x=100, y=1).exists())
        self.assertFalse(RoutePoint.objects.filter(id=self.point.id, x=69.0, y=42).exists())

class SerializationValidationTests(APITestCase):
    def setUp(self):
        self.user = get_user_model().objects.create_user(username='testuser', password='testpass')
        self.token = Token.objects.create(user=self.user)
        self.client.credentials(HTTP_AUTHORIZATION='Token ' + self.token.key)

        self.image = BackgroundImage.objects.create(name='img', image='test.jpg')

    def test_invalid_route_data(self):
        response = self.client.post('/api/routes/', {
            'name': '',
            'background': ''
        })

        self.assertEqual(response.status_code, 400)

    def test_invalid_point_data(self):
        route = Route.objects.create(name='route', user=self.user, background=self.image)
        response = self.client.post(f'/api/routes/{route.id}/points/', {
            'x': -1,
            'y': 101,
        })

        self.assertEqual(response.status_code, 400)
        