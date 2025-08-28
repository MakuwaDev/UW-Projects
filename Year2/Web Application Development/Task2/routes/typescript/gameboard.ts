import { getCSRFToken, Dot, GameBoard  } from './main.js';

document.addEventListener("DOMContentLoaded", () => {
    console.log("Gameboard script loaded");

    let board : GameBoard = {
        pk: null,
        title: null,
        rows: 0,
        cols: 0,
        dots: []
    };

    const container = document.getElementById("grid-container") as HTMLDivElement;
    const generate_button = document.getElementById("generate") as HTMLButtonElement;
    const titleInput = document.getElementById("title") as HTMLInputElement;
    const rowsInput = document.getElementById("rows") as HTMLInputElement;
    const colsInput = document.getElementById("cols") as HTMLInputElement;
    const errorMessage = document.getElementById("error-message") as HTMLDivElement;
    const hidden = document.getElementById("hidden-form") as HTMLFormElement;

    if (hidden) {
        const pk = document.getElementById("hidden-pk") as HTMLInputElement;
        const title = document.getElementById("hidden-title") as HTMLInputElement;
        const rows = document.getElementById("hidden-rows") as HTMLInputElement;
        const cols = document.getElementById("hidden-cols") as HTMLInputElement;
        const dots = (document.getElementById("hidden-dots") as HTMLInputElement).value;
        
        board.pk = parseInt(pk.value);
        board.title = title.value;
        board.rows = parseInt(rows.value);
        board.cols = parseInt(cols.value);

        console.log(`Loaded board with pk: ${board.pk}, title: ${board.title}, rows: ${board.rows}, cols: ${board.cols}`);

        titleInput.value = board.title || "";
        rowsInput.value = board.rows.toString();
        colsInput.value = board.cols.toString();

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

        container.innerHTML = "";

        container.style.display = "grid";
        container.style.gridTemplateColumns = `repeat(${board.cols}, 80px)`;
        container.style.gridTemplateRows = `repeat(${board.rows}, 80px)`;

        for (let row = 0; row < board.rows; ++row) {
            for (let col = 0; col < board.cols; ++col) {
                const cell = document.createElement('div');
                cell.className = "grid-cell";
                cell.dataset.row = row.toString();
                cell.dataset.col = col.toString();

                cell.addEventListener('click', () => {
                    console.log(`Cell clicked}`);

                    errorMessage.textContent = "";

                    const dot: Dot = {
                        row: parseInt((cell as HTMLElement).dataset.row!),
                        col: parseInt((cell as HTMLElement).dataset.col!),
                        color: color || "",
                    }

                    const existing = board.dots.find(d => d.row === dot.row && d.col === dot.col);
                    if (existing) {
                        const index = board.dots.indexOf(existing);
                        board.dots.splice(index, 1);
                        cell.innerHTML = "";
                        return;
                    }

                    if (color) {
                        const colorCount = board.dots.filter(d => d.color === color).length;
                        if (colorCount == 2) {
                            errorMessage.textContent = "You can only place two dots of the same color.";
                            return;
                        }

                        board.dots.push(dot);
                        const dotElement = document.createElement('div');
                        dotElement.className = "dot";
                        dotElement.style.background = color;
                        dotElement.style.gridRowStart = (dot.row + 1).toString();
                        dotElement.style.gridColumnStart = (dot.col + 1).toString();
                        cell.appendChild(dotElement);
                    } else {
                        errorMessage.textContent = "Please select a color first.";
                    }
                });

                container.appendChild(cell);

                if (board.dots.filter(d => d.row === row && d.col === col).length > 0) {
                    const dot = board.dots.find(d => d.row === row && d.col === col);
                    if (dot) {
                        const dotElement = document.createElement('div');
                        dotElement.className = "dot";
                        dotElement.style.background = dot.color;
                        dotElement.style.gridRowStart = (dot.row + 1).toString();
                        dotElement.style.gridColumnStart = (dot.col + 1).toString();
                        cell.appendChild(dotElement);
                    }
                }
            }
        }
    }

    let color : string | null = null;

    document.querySelectorAll('.color-button').forEach(button => {
        button.addEventListener('click', () => {
            color = (button as HTMLButtonElement).dataset.color!;

            console.log(`Selected color: ${color}`);
        });
    });

    generate_button.addEventListener("click", () => {
        console.log("Generate button clicked");

        board.title = titleInput.value;
        board.cols = parseInt(colsInput.value);
        board.rows = parseInt(rowsInput.value);
        board.dots = [];

        if (isNaN(board.rows) || isNaN(board.cols) || board.rows == 0 || board.cols == 0) {
            errorMessage.textContent = "Invalid board size. Please enter valid numbers for rows and columns.";
            return;
        }

        if (container) {
            console.log(`Generating grid with ${board.rows} rows and ${board.cols} columns`);
            container.innerHTML = "";

            container.style.display = "grid";
            container.style.gridTemplateColumns = `repeat(${board.cols}, 80px)`;
            container.style.gridTemplateRows = `repeat(${board.rows}, 80px)`;

            for (let row = 0; row < board.rows; ++row) {
                for (let col = 0; col < board.cols; ++col) {
                    const cell = document.createElement('div');
                    cell.className = "grid-cell";
                    cell.dataset.row = row.toString();
                    cell.dataset.col = col.toString();

                    cell.addEventListener('click', () => {
                        console.log(`Cell clicked}`);
                        if (color) {
                            errorMessage.textContent = "";

                            const dot: Dot = {
                                row: parseInt((cell as HTMLElement).dataset.row!),
                                col: parseInt((cell as HTMLElement).dataset.col!),
                                color: color
                            }

                            const existing = board.dots.find(d => d.row === dot.row && d.col === dot.col);
                            if (existing) {
                                const index = board.dots.indexOf(existing);
                                board.dots.splice(index, 1);
                                cell.innerHTML = "";
                                return;
                            }

                            const colorCount = board.dots.filter(d => d.color === color).length;
                            if (colorCount == 2) {
                                errorMessage.textContent = "You can only place two dots of the same color.";
                                return;
                            }

                            board.dots.push(dot);
                            const dotElement = document.createElement('div');
                            dotElement.className = "dot";
                            dotElement.style.background = color;
                            dotElement.style.gridRowStart = (dot.row + 1).toString();
                            dotElement.style.gridColumnStart = (dot.col + 1).toString();
                            cell.appendChild(dotElement);
                        } else {
                            errorMessage.textContent = "Please select a color first.";
                        }
                    });

                    container.appendChild(cell);
                }
            }
        }
    });

    document.getElementById("board-form")!.addEventListener("submit", async (e) => {
        e.preventDefault();

        const response = await fetch("/routes/gameboard_save/", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "X-CSRFToken": getCSRFToken(),
            },
            body: JSON.stringify(board),
        });

        if (response.ok) {
            console.log("Board saved successfully, redirecting...");
            window.location.href = "/routes/my_gameboards/";
        } else {
            console.log(response.text)
            alert("Failed to save board. Please try again.");
        }
    });
});