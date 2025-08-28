from django import template


register = template.Library()


@register.filter
def get_item(list_like, index):
    try:
        return list_like[index]
    except:
        return None
    
@register.filter
def attr(obj, attribute_name):
    return getattr(obj, attribute_name, None)