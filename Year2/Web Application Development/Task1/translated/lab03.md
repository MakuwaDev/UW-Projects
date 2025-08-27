# Description

In this lab, the goal is to use `flex` and `scss` to build a simple layout for the webpage from the previous class.

## Requirements

Demonstrate:

1. Hiding one of the page elements when the browser window width is smaller than `800px`.
2. Changing the background color of a page element when the cursor is hovering over it.
3. Changing the background color of a page element on hover, but **only** when the browser window width is greater than `800px`.
4. Reordering page elements when the window width is less than `800px` (elements should stack vertically).
5. Using the [justify-content](https://developer.mozilla.org/en-US/docs/Web/CSS/justify-content) attribute to align elements in a container:
   - to the left,
   - to the right,
   - centered,
   - evenly distributed.
6. Preparing `SCSS` that includes:
   - a variable for background colors,
   - a variable for text colors,
   - a variable for link colors.
7. Using variables in the `SCSS` file.
8. Using mixins in the `SCSS` file.
9. Using nesting in the `SCSS` file.
10. Using functions in the `SCSS` file (e.g. [`color.scale()`](https://sass-lang.com/documentation/breaking-changes/color-functions/#single-channel-adjustment-functions)).

## Elegant CSS

**SCSS** is an extension of CSS syntax that allows using variables, nesting, and mixins. **Sass** is a CSS preprocessor that compiles SCSS into standard CSS.

### Installation and running

> **Note:** Make sure you have `npm` installed before continuing.

#### Using Live Sass Compiler in VSCode
The simplest way to work with SCSS is to install the **Live Sass Compiler** extension in VSCode. Once installed, click the `Watch Sass` button in the bottom-right status bar to automatically compile SCSS into CSS on every save.  
If you don’t like using VSCode and prefer editors like VIM 😎, follow the instructions below.

#### Global installation with npm
> **Note:** Global installation doesn’t work on lab computers due to lack of permissions.

Install Sass globally with:
```bash
npm install -g sass
```
After installation, use the `sass` command anywhere to compile SCSS to CSS:
```bash
sass [file.scss] [file.css]
```


#### Local installation with npm
To install Sass locally, initialize a project::
```bash
npm init -y
```
Then install Sass in the same folder:
```bash
npm install sass
```
Compile SCSS to CSS with:
```bash
npx sass [plik.scss] [plik.css]
```

### Automatic compilation on save
To automatically compile SCSS to CSS on every change, use the `--watch` flag:
```bash
sass --watch [plik.scss]:[plik.css]
```
Or for local installation:
```bash
npx sass --watch [plik.scss]:[plik.css]
```

## Helpful resources

- [Flexbox](https://css-tricks.com/snippets/css/a-guide-to-flexbox/)
- [SCSS](https://sass-lang.com/guide)
- [SCSS Variables](https://sass-lang.com/documentation/variables)
- [Media queries](https://developer.mozilla.org/en-US/docs/Web/CSS/Media_Queries/Using_media_queries)
- [Pseudo-classes](https://developer.mozilla.org/en-US/docs/Web/CSS/Pseudo-classes)
- [Pseudo-elements](https://developer.mozilla.org/en-US/docs/Web/CSS/Pseudo-elements)
- [CSS Selectors](https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Selectors)
- [CSS Variables](https://developer.mozilla.org/en-US/docs/Web/CSS/Using_CSS_custom_properties)
