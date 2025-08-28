from django import forms
from .models import Route, RoutePoint


class RouteForm(forms.ModelForm):
    class Meta:
        model = Route
        fields = ["name", "background"]


class RoutePointForm(forms.ModelForm):
    class Meta:
        model = RoutePoint
        fields = ["x", "y"]

        def clean_x(self):
            x = self.cleaned_data["x"]
            if not (0 <= x <= 100):
                raise forms.ValidationError("X coordinate must be between 0 and 100.")
            return x
        
        def clean_y(self):
            y = self.cleaned_data["y"]
            if not (0 <= y <= 100):
                raise forms.ValidationError("Y must be between 0 and 100.")
            return y