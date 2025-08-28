import { getCSRFToken, Dot, GameBoard  } from './main.js';

function getCellCenterPosition(col: number, row: number): { x: number; y: number } {
  const x = col * 80;
  const y = row * 80;

  return { x: x + 40, y: y + 40 };
}

function drawPathOnSVG(pathDots: Dot[], color: string) {
    const svg = document.getElementById("path-svg") as unknown as SVGSVGElement;
    if (!svg) {
        return;
    }

    const existingPath = svg.querySelector(`path[data-color="${color}"]`);
    if (existingPath) {
        existingPath.remove();
    }

    if (pathDots.length < 2) {
        return;
    }

    let d = "";

    pathDots.forEach((dot, i) => {
    const pos = getCellCenterPosition(dot.col, dot.row);
    if (i === 0) {
        d += `M ${pos.x} ${pos.y} `;
    } else {
        d += `L ${pos.x} ${pos.y} `;
    }
    });

    const path = document.createElementNS("http://www.w3.org/2000/svg", "path");
    path.setAttribute("d", d.trim());
    path.setAttribute("stroke", color);
    path.setAttribute("stroke-width", "20");
    path.setAttribute("fill", "none");
    path.setAttribute("stroke-linejoin", "round");
    path.setAttribute("stroke-linecap", "round");
    path.dataset.color = color;

    svg.appendChild(path);
}

document.addEventListener("DOMContentLoaded", () => {
    console.log("Path script loaded");

    document.addEventListener("dragstart", (e) => {
        e.preventDefault();
    });

    let board : GameBoard = {
        pk: null,
        title: null,
        rows: 0,
        cols: 0,
        dots: []
    };

    let isDrawing = false;
    let currentColor: string | null = null;
    let currentPath: Dot[] = [];
    let paths: Dot[] = [];

    function highlightCell(cell: HTMLDivElement) {
        const highlight = document.createElement("div");
        highlight.dataset.color = currentColor || "#FFFFFF";
        highlight.dataset.col = cell.dataset.col!;
        highlight.dataset.row = cell.dataset.row!;
        highlight.className = "path-highlight";
        highlight.style.background = currentColor || "#FFFFFF";

        highlight.addEventListener("click", () => {
            const tmpPath = paths.filter(p => p.color === highlight.dataset.color!);
            clearHighlights(highlight.dataset.color!);

            const cells = (Array.from(
                                    document.
                                    getElementsByClassName("grid-cell")) as HTMLDivElement[])
                                    .filter(c => (
                                        c.dataset.col === tmpPath[0].col.toString() &&
                                        c.dataset.row === tmpPath[0].row.toString()) || (
                                        c.dataset.col === tmpPath[tmpPath.length - 1].col.toString() &&
                                        c.dataset.row === tmpPath[tmpPath.length - 1].row.toString()));
            
            cells.forEach(
                (cell) => {
                    const handler = mouseDownHandlers.get(cell);
                    if (handler) {
                        cell.addEventListener("mousedown", handler);
                    }
                }
            )
        })

        cell.appendChild(highlight);

        drawPathOnSVG(currentPath, currentColor || "#FFFFFF");
    }

    const mouseDownHandlers = new Map<HTMLDivElement, EventListener>();

    function createMouseDownHandler(
        row: number,
        col: number,
        dot: Dot,
        cell: HTMLDivElement
    ): (event: Event) => void {
        return function (event: Event) {
            const mouseEvent = event as MouseEvent;
            isDrawing = true;
            currentColor = dot.color;
            currentPath.push({
                row,
                col,
                color: currentColor
            });
            highlightCell(cell);
            document.addEventListener("mousemove", addToPath);
            document.addEventListener("mouseup", cancelPath);
        };
    }

    function clearHighlights(color: string) {
        paths = paths.filter(p => p.color != color);
        Array.from(document.getElementsByClassName("path-highlight")).forEach(
            (element) => {
                if ((element as HTMLDivElement).dataset.color == color) {
                    element.remove();
                }
            }
        );

        const svg = document.getElementById("path-svg") as unknown as SVGSVGElement;
        if (!svg) {
            return;
        }

        const existingPath = svg.querySelector(`path[data-color="${color}"]`);
        if (existingPath) {
            existingPath.remove();
        }
    }

    function cancelPath() {
        if (isDrawing) {
            isDrawing = false;
            clearHighlights(currentColor || "#FFFFFF");
            currentColor = null;
            currentPath = [];
            document.removeEventListener("mouseup", cancelPath);
            document.removeEventListener("mousemove", addToPath);
        }
    }

    function finalizePath() {
        currentPath.forEach(
            (element) => {
                paths.push({
                    row: element.row,
                    col: element.col,
                    color: currentColor || "#FFFFFF"
                })
            }
        );

        const cells = (Array.from(
                                document.
                                getElementsByClassName("grid-cell")) as HTMLDivElement[])
                                .filter(c =>(
                                    c.dataset.col === currentPath[0].col.toString() &&
                                    c.dataset.row === currentPath[0].row.toString()) || (
                                    c.dataset.col === currentPath[currentPath.length - 1].col.toString() &&
                                    c.dataset.row === currentPath[currentPath.length - 1].row.toString()));
        
        cells.forEach(
            (cell) => {
                const handler = mouseDownHandlers.get(cell);
                if (handler) {
                    cell.removeEventListener("mousedown", handler);
                }
            }
        )

        currentColor = null;
        currentPath = [];

        document.removeEventListener("mouseup", finalizePath);
    }

    function addToPath(e: MouseEvent) {
        if (!isDrawing) {
            return;
        }

        const x = e.clientX;
        const y = e.clientY;

        const element = document.elementFromPoint(x, y);
        if (!element) {
            return;
        }

        const cell = element.closest('.grid-cell') as HTMLDivElement | null;
        if (!cell) {
            return;
        }

        const row = parseInt(cell.dataset.row!);
        const col = parseInt(cell.dataset.col!);
        if (currentPath.some(p => p.row === row && p.col === col)) {
            return;
        }

        const last = currentPath.slice(-1)[0];
        if (!last) {
            return;
        }

        const dx = row - last.row;
        const dy = col - last.col;

        const isAdjacent = (dx === 0 && Math.abs(dy) === 1) || (dy === 0 && Math.abs(dx) === 1);
        if (!isAdjacent) {
            return;
        }

        if (paths.some(p => p.col === col && p.row === row)) {
            return;
        }

        let pathRoot = currentPath[0];
        let currentDot = board.dots.find(d => d.row === row && d.col === col);
        if (currentDot != null && currentDot.color != currentColor) {
            return;
        } else if (currentDot != null && currentDot != pathRoot) {
            document.removeEventListener("mousemove", addToPath);
            document.removeEventListener("mouseup", cancelPath);
            document.addEventListener("mouseup", finalizePath);
        }

        currentPath.push({
            col: col,
            row: row,
            color: currentColor || "#FFFFFF"
        });
        highlightCell(cell);
    }

    const container = document.getElementById("grid-container") as HTMLDivElement;
    const hidden_board = document.getElementById("hidden-board-form") as HTMLFormElement;
    const hidden_path = (document.getElementById("hidden-path-cells") as HTMLFormElement).value;

    if (hidden_board) {
        const pk = document.getElementById("hidden-board-pk") as HTMLInputElement;
        const title = document.getElementById("hidden-board-title") as HTMLInputElement;
        const rows = document.getElementById("hidden-board-rows") as HTMLInputElement;
        const cols = document.getElementById("hidden-board-cols") as HTMLInputElement;
        const dots = (document.getElementById("hidden-board-dots") as HTMLInputElement).value;
        
        board.pk = parseInt(pk.value);
        board.title = title.value;
        board.rows = parseInt(rows.value);
        board.cols = parseInt(cols.value);

        let parsedDots: Dot[] = [];

        try {
            parsedDots = JSON.parse(dots);
        } catch (error) {
            console.error("Failed to parse hidden dots JSON:", error);
        }

        board.dots = parsedDots.map(d => ({
            row: d.row,
            col: d.col,
            color: d.color
        }));

        container.style.display = "grid";
        container.style.gridTemplateColumns = `repeat(${board.cols}, 80px)`;
        container.style.gridTemplateRows = `repeat(${board.rows}, 80px)`;

        let parsedPath: Dot[] = [];

        try {
            parsedPath = JSON.parse(hidden_path);
        } catch (error) {
            console.error("Failed to parse hidden path JSON:", error);
        }

        parsedPath.forEach(
            (element) => {
                paths.push({
                    row: element.row,
                    col: element.col,
                    color: element.color || "#FFFFFF"
                })
            }
        );

        for (let row = 0; row < board.rows; ++row) {
            for (let col = 0; col < board.cols; ++col) {
                const cell = document.createElement('div');
                cell.className = "grid-cell";
                cell.dataset.row = row.toString();
                cell.dataset.col = col.toString();

                container.appendChild(cell);

                const dot = board.dots.find(d => d.row === row && d.col === col);
                if (dot) {
                    const dotElement = document.createElement('div');
                    dotElement.className = "dot";
                    dotElement.style.backgroundColor = dot.color;
                    dotElement.style.gridRowStart = (dot.row + 1).toString();
                    dotElement.style.gridColumnStart = (dot.col + 1).toString();

                    
                    const handler = createMouseDownHandler(row, col, dot, cell);
                    cell.addEventListener("mousedown", handler);
                    mouseDownHandlers.set(cell, handler);

                    cell.appendChild(dotElement);
                }
            }
        }

        const svg = document.getElementById("path-svg") as unknown as SVGSVGElement;
        if (svg) {
            svg.innerHTML = ""

            const pathsByColor = new Map<string, Dot[]>();
            for (const dot of parsedPath) {
                if (!pathsByColor.has(dot.color)) {
                    pathsByColor.set(dot.color, []);
                }

                pathsByColor.get(dot.color)!.push(dot);
            }

            pathsByColor.forEach((dots, color) => {
                currentColor = color;
                dots.forEach((dot) => {
                    const cellArray = Array.from(document.getElementsByClassName("grid-cell")) as HTMLDivElement[];
                    const cell = cellArray!.find(c => 
                                            c.dataset.col === dot.col.toString() &&
                                            c.dataset.row === dot.row.toString());
                    if (cell) {
                        currentPath.push(dot);
                        highlightCell(cell);
                    }
                });
                currentPath = [];
            });

            currentColor = null;
        }
    }

    document.getElementById("path-form")!.addEventListener("submit", async (e) => {
        e.preventDefault();

        const pk = document.getElementById("hidden-path-pk") as HTMLInputElement;

        const response = await fetch(`/routes/path_save/${pk.value}/`, {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "X-CSRFToken": getCSRFToken(),
            },
            body: JSON.stringify(paths),
        });

        if (response.ok) {
            console.log("Path saved successfully, redirecting...");
            window.location.href = "/routes/my_paths/";
        } else {
            console.log(response.text)
            alert("Failed to save path. Please try again.");
        }
    });
});