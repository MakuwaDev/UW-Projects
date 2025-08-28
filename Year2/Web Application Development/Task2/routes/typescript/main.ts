export function getCSRFToken(): string {
    return (document.querySelector('[name=csrfmiddlewaretoken]') as HTMLInputElement).value;
}

export interface Dot {
    row: number;
    col : number;
    color: string;
}

export interface GameBoard {
    pk: number | null;
    title: string | null;
    rows: number;
    cols: number;
    dots: Dot[];
}