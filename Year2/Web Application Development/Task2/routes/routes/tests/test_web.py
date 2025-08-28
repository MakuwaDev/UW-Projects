from django.test import TestCase
from django.contrib.auth import get_user_model
from django.urls import reverse
from routes.models import Route, RoutePoint, BackgroundImage

class AuthenticationTests(TestCase):
    def setUp(self):
        self.user1 = get_user_model().objects.create_user(username='testuser1', password='testpass')
        self.user2 = get_user_model().objects.create_user(username='testuser2', password='testpass')

        self.image = BackgroundImage.objects.create(name='Test Image', image='test.jpg')
        self.route1 = Route.objects.create(name='Test Route', user=self.user1, background=self.image)
        self.route2 = Route.objects.create(name='Test Route', user=self.user2, background=self.image)
        self.route_point = RoutePoint.objects.create(route=self.route1, x=69, y=42, order=0)

        self.main_url = reverse('routes:index')
        self.login_url = reverse('routes:login')
        self.logout_url = reverse('routes:logout')
        self.register_url = reverse('routes:register')
        self.route_list_url = reverse('routes:my_routes')

    def test_access_protected_page_requires_login(self):
        for url in [
            self.main_url,
            self.route_list_url,
            reverse('routes:route_detail', args=[self.route1.id]),
            reverse('routes:route_detail', args=[self.route2.id])]:
            response = self.client.get(url)
            self.assertRedirects(response, f'{self.login_url}?next={url}')

    def test_login_functionality(self):
        response = self.client.post(self.login_url, {'username': 'testuser1', 'password': 'testpass'})
        self.assertRedirects(response, self.main_url)
        self.assertTrue('_auth_user_id' in self.client.session)

    def test_logout_functionality(self):
        self.client.login(username='testuser', password='testpass')

        response = self.client.get(self.logout_url)
        self.assertRedirects(response, self.login_url)
        self.assertFalse('_auth_user_id' in self.client.session)

    ### TODO Add register test

    def test_access_to_other_users_route(self):
        self.client.login(username='testuser1', password='testpass')
        
        route2_detail_url = reverse('routes:route_detail', args=[self.route2.id])
        response = self.client.get(route2_detail_url)
        self.assertRedirects(response, self.route_list_url)

    def test_add_point_to_other_users_route(self):
        self.client.login(username='testuser1', password='testpass')

        route2_detail_url = reverse('routes:route_detail', args=[self.route2.id])
        response = self.client.post(route2_detail_url, {
            'x': 60, 'y': 60, 'order': 1
        })
        self.assertRedirects(response, self.route_list_url)


    def test_delete_other_users_point(self):
        self.client.login(username='testuser2', password='testpass')

        delete_point_url = reverse('routes:point_delete', args=[self.route_point.id])
        response = self.client.post(delete_point_url)

        self.assertRedirects(response, self.route_list_url)
        self.assertTrue(RoutePoint.objects.filter(id=self.route_point.id).exists())

    def test_delete_other_users_route(self):
        self.client.login(username='testuser1', password='testpass')

        delete_route_url = reverse('routes:route_delete', args=[self.route2.id])
        response = self.client.post(delete_route_url)

        self.assertRedirects(response, self.route_list_url)
        self.assertTrue(Route.objects.filter(id=self.route2.id).exists())


class managementTests(TestCase):
    def setUp(self):
        self.user1 = get_user_model().objects.create_user(username='testuser1', password='testpass')
        self.user2 = get_user_model().objects.create_user(username='testuser2', password='pass')
        self.image = BackgroundImage.objects.create(name='img', image='test.jpg')
        self.route1 = Route.objects.create(name='route1', user=self.user1, background=self.image)
        self.route2 = Route.objects.create(name='route2', user=self.user2, background=self.image)
        self.route_point1 = RoutePoint.objects.create(route=self.route1, x=1, y=2)
        self.route_point2 = RoutePoint.objects.create(route=self.route2, x=3, y=4)

        self.my_routes_url = reverse('routes:my_routes')

    def test_my_routes_view(self):
        self.client.login(username='testuser1', password='testpass')
        response = self.client.get(self.my_routes_url)

        self.assertEqual(response.status_code, 200)
        self.assertContains(response, self.route1.name)
        self.assertNotContains(response, self.route2.name)

    def test_route_detail_view(self):
        self.client.login(username='testuser1', password='testpass')
        response = self.client.get(reverse('routes:route_detail', args=[self.route1.id]))

        self.assertEqual(response.status_code, 200)

        self.assertContains(response, self.route1.background.image.name)

        self.assertContains(response, self.route1.name)
        self.assertNotContains(response, self.route2.name)

        self.assertContains(response, '(1.0, 2.0)')
        self.assertNotContains(response, '(3.0, 4.0)')

    def test_add_point(self):
        self.client.login(username='testuser1', password='testpass')
        route_detail_url = reverse('routes:route_detail', args=[self.route1.id])
        response = self.client.post(route_detail_url, {
            'x': 5, 'y': 6, 'add_point': 1
        })

        self.assertRedirects(response, reverse('routes:route_detail', args=[self.route1.id]))
        self.assertTrue(RoutePoint.objects.filter(route=self.route1, x=5, y=6).exists())

        response = self.client.get(reverse('routes:route_detail', args=[self.route1.id]))
        self.assertContains(response, '5')
        self.assertContains(response, '6')

    def test_delete_point(self):
        self.client.login(username='testuser1', password='testpass')
        response = self.client.post(reverse('routes:point_delete', args=[self.route_point1.id]))

        self.assertRedirects(response, reverse('routes:route_detail', args=[self.route1.id]))

        response = self.client.get(reverse('routes:route_detail', args=[self.route1.id]))

        self.assertFalse(RoutePoint.objects.filter(id=self.route_point1.id).exists())
        self.assertNotContains(response, '(1.0, 2.0)')
        self.assertContains(response, self.route1.name)

    def test_create_route(self):
        self.client.login(username='testuser1', password='testpass')
        response = self.client.post(reverse('routes:new_route'), {
            'name': 'New Route',
            'background': self.image.id,
            'save_route': 1,
        })

        self.assertEqual(Route.objects.filter(name='New Route').count(), 1)
        new_route = Route.objects.get(name='New Route')

        self.assertEqual(new_route.user, self.user1)
        self.assertRedirects(response, reverse('routes:route_detail', args=[new_route.id]))

        response = self.client.get(reverse('routes:route_detail', args=[new_route.id]))

        self.assertContains(response, 'New Route')
        self.assertContains(response, self.image.image.name)

    def test_delete_route(self):
        self.client.login(username='testuser1', password='testpass')
        response = self.client.post(reverse('routes:route_delete', args=[self.route1.id]))

        self.assertRedirects(response, reverse('routes:my_routes'))
        self.assertFalse(Route.objects.filter(id=self.route1.id).exists())
        self.assertTrue(Route.objects.filter(id=self.route2.id).exists())

        self.assertFalse(RoutePoint.objects.filter(route=self.route1).exists())
        self.assertTrue(RoutePoint.objects.filter(route=self.route2).exists())

    def test_delete_invalid_point(self):
        self.client.login(username='testuser1', password='testpass')
        response = self.client.post(reverse('routes:point_delete', args=[69]))

        self.assertEqual(response.status_code, 404)

    def test_delete_invalid_route(self):
        self.client.login(username='testuser1', password='testpass')
        response = self.client.post(reverse('routes:route_delete', args=[69]))

        self.assertEqual(response.status_code, 404)