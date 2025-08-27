# Goal

- The goal of this assignment is to prepare a website in HTML format. Ideally, the page should relate to the topic chosen for the first lab.
- An important part of the task is installing `npm` and [https://www.npmjs.com/package/html-validate](https://www.npmjs.com/package/html-validate)

# Requirements

1. Page structure:
   - Add a header, article, and footer to the page ([`<header>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/header), [`<article>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/article), and [`<footer>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/footer)).
   - Use [`<h1>` and `<h2>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/Heading_Elements) tags for headings.
2. Table:
   - Create a [`<table>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/table) with some data (can be made-up).
   - The table should include a header [`<thead>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/thead), rows [`<tr>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/tr), and columns [`<th>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/th) with data.
   - Add a title to the table using [`<caption>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/caption).
3. List:
   - Create an ordered list [`<ol>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/ol) numbered with Roman numerals.
4. Text emphasis:
   - Use tags to highlight text (e.g., [`<strong>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/strong), [`<em>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/em)).
5. Image:
   - Add an image to the page ([`<img>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/img)).
6. Form:
   - Create a form ([`<form>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/form)) that includes, among others:
       - A text field ([`<input>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input)).
       - A dropdown ([`<select>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/select)) with [optgroup](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/select#select_with_grouping_options).
       - A button ([`<button>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/button)).
7. Collapsible description:
   - Use [`<details>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/details) and [`<summary>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/summary) to create a collapsible description.
8. Progress bar:
   - Use the [`<progress>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/progress) element to display progress (e.g., task completion progress).
9. Image map:
   - Create a meaningful image map using [`<map>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/map) and [`<area>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/area). The map should contain at least 3 areas (`<area>`).

# Publishing

The page should be published the same way as in [the first lab assignment](https://kciebiera.github.io/www-2425/lab1.html#wystawianie).

# Validating the page

The page should be validated using the `html-validate` tool. To do this, install it via `npm` in one of two ways:

- globally: `npm install -g html-validate`. This allows you to validate `.html` files by running `html-validate [file]` or `html-validate [folder]` (recursively validates all `.html` files in a folder).
- locally: Create a project with `npm init -y`, then install the tool in the same directory using `npm install html-validate`. Now you can validate with `npx html-validate [file/folder]`. Alternatively, add a `scripts` entry to your `package.json`:
   ```json
   "scripts": {
      "validate": "html-validate ."
   }
  ```
and then validate the entire project with `npm run validate`.

# Helpful resources

- [HTML](https://developer.mozilla.org/en-US/docs/Web/HTML/Element)
- [HTML - tables](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/table)

# Notes

1. Focus on correct use of HTML tags, not on the visual design of the page.
2. Ensure readability and correctness of the code.
3. If you have problems, use the provided resources or contact your instructor.

# Additional information

You can extend the assignment with extra elements, for example:

- Adding links to other pages.
- Embedding a video.
- Using inline styles to format text.

GLHF!
