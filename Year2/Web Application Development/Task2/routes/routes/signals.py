from django.db.models.signals import post_save
from django.dispatch import receiver
from threading import Lock

from .models import GameBoard, Path
from .views import events, current_id

lock = Lock()

@receiver(post_save, sender=GameBoard)
def board_created(sender, instance, created, **kwargs):
    if created:
        with lock:
            current_id[0] += 1
            events.append({
                "type": "newBoard",
                "data": {
                    "board_id": instance.pk,
                    "board_name": instance.title,
                    "creator_username": instance.user.username if instance.user else "unknown"
                }
            })


@receiver(post_save, sender=Path)
def path_created(sender, instance, created, **kwargs):
    if created:
        with lock:
            current_id[0] += 1
            events.append({
                "type": "newPath",
                "data": {
                    "path_id": instance.pk,
                    "board_id": instance.board.pk,
                    "board_name": instance.board.title,
                    "user_username": instance.user.username if instance.user else "unknown"
                }
            })