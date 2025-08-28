document.addEventListener('DOMContentLoaded', () => {
    document.querySelectorAll('.point-item').forEach(item => {
        let marker: HTMLDivElement | null = null;

        item.addEventListener('mouseenter', () => {
            const element = item as HTMLElement;

            const x = parseInt(element.dataset.x!);
            const y = parseInt(element.dataset.y!);

            console.log(`Highlighting point at (${x}, ${y})`);

            if (marker) {
                marker.remove();
            }

            marker = document.createElement('div');
            marker.style.position = 'absolute';
            marker.style.top = `${y}%`;
            marker.style.left = `${x}%`;
            marker.style.zIndex = '1000';
            marker.style.width = '15px';
            marker.style.height = '15px';
            marker.style.background = 'blue';
            marker.style.borderRadius = '50%';
            marker.style.transform = 'translate(-50%, -50%)';
            marker.style.pointerEvents = 'none';

            const container = document.getElementById('image-container');
            container?.appendChild(marker);
        });

        item.addEventListener('mouseleave', () => {
            setTimeout(() => {marker?.remove(); marker = null}, 500);
            });
    });
});