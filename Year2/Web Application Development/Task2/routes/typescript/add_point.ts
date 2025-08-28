document.addEventListener('DOMContentLoaded', () => {

    const image = document.getElementById('image-container') as HTMLImageElement;
    const inputX = document.getElementById('id_x') as  HTMLInputElement;
    const inputY = document.getElementById('id_y') as HTMLInputElement;
    const svg = document.getElementById('route-svg') as unknown as SVGSVGElement;
    const lastPoint = document.getElementById('last-point') as HTMLDivElement;

    let marker: HTMLDivElement | null = null;
    let line: SVGLineElement | null = null;

    image.addEventListener('click', (e: MouseEvent) => {

        const rect = image.getBoundingClientRect();
        const x = ((e.clientX - rect.left) / rect.width) * 100;
        const y = ((e.clientY - rect.top) / rect.height) * 100;

        inputX.value = Math.round(x).toString();
        inputY.value = Math.round(y).toString();

        if (!marker) {
            marker = document.createElement('div');

            marker.style.position = 'absolute';
            marker.style.zIndex = '1';
            marker.style.transform = 'translate(-50%, -50%)';
            marker.style.width = '10px';
            marker.style.height = '10px';
            marker.style.background = 'red';
            marker.style.borderRadius = '50%';

            image?.appendChild(marker);
        }

        if (!line) {
            line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
            line.setAttribute('stroke', 'blue');
            line.setAttribute('stroke-width', '0.5');
            line.setAttribute('x1', lastPoint.dataset.x!);
            line.setAttribute('y1', lastPoint.dataset.y!);
            svg.appendChild(line);
        }

        marker.style.top = `${y}%`;
        marker.style.left = `${x}%`;
        line.setAttribute('x2', `${x}`);
        line.setAttribute('y2', `${y}`);
    });
});