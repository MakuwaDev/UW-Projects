const source = new EventSource("/routes/sse/notifications/");

source.onopen = () => {
    console.log("SSE connection established");
};

source.addEventListener("newBoard", (e: Event) => {
    const data = JSON.parse((e as MessageEvent).data);
    showNotification(`🆕 ${data.creator_username} has created a new board: ${data.board_name}.`,
                        "new-board");
});

source.addEventListener("newPath", (e: Event) => {
    const data = JSON.parse((e as MessageEvent).data);
    showNotification(`📍 ${data.user_username} has entered: ${data.board_name} for the first time.`,
                        "new-path");
});

source.onerror = (err) => {
    console.error("SSE connection error:", err);
    showNotification("⚠️ Connection to the SSE server lost.", "error");
};

function showNotification(message: string, type: "error" | "new-board" | "new-path") {
    const toast = document.createElement("div");
    toast.textContent = message;
    toast.className = "toast"
    toast.classList.add(type);

    document.body.appendChild(toast);

    setTimeout(() => {
        toast.remove();
    }, 5000);
}