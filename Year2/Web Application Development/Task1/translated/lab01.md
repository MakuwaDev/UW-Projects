# Task

In this assignment, you need to write a program that prepares a static website and publishes it “on the internet”.

More precisely:

1. Write a program that downloads a web page dedicated to a topic of your choice, for example:

- TikTokers ([https://www.favikon.com/blog/the-20-most-famous-tiktok-influencers-in-the-world](https://www.favikon.com/blog/the-20-most-famous-tiktok-influencers-in-the-world))
- programming languages ([https://www.tiobe.com/tiobe-index/](https://www.tiobe.com/tiobe-index/))
- chess openings ([https://chessfox.com/chess-openings-list/](https://chessfox.com/chess-openings-list/))

  **Important information**: this page should contain a list of elements that can be easily scraped (e.g., a list of programming languages, a list of chess openings, a list of TikTok influencers). This list may be in the form of a table, list, etc.

2. The program should process the web page and generate a file in markdown format containing the list of elements scraped from the internet along with some additional information (e.g., description, link, image, etc.).
3. Then, using a library such as [https://github.com/MarioVilas/googlesearch](https://github.com/MarioVilas/googlesearch) or [https://github.com/deedy5/duckduckgo_search](https://github.com/deedy5/duckduckgo_search), search the internet for additional information about each scraped element.
4. Finally, prepare a website in markdown format that includes a static page, images, the scraped data (see point 2), and any other information you consider relevant.

## Website

The website should include:

- at least one static page describing your chosen topic
- the list scraped from the internet
- a subpage for each item

## Deployment

The website should be generated using [Jekyll](https://jekyllrb.com/) or something similar (e.g., [Hugo](https://gohugo.io/)).  
It should be published on GitHub Pages or on *students*.

Publishing on GitHub Pages is described here: [https://pages.github.com/](https://pages.github.com/)

## Python tips for beginners

If you don’t know Python, we recommend the [official tutorial](https://docs.python.org/3/tutorial/).

Before you start installing Python libraries, make sure to create a virtual environment (`venv`). It isolates your project’s dependencies from the global Python packages in your system, so that:

- you avoid clutter,
- you don’t break system packages,
- you make it easier to reproduce the environment and run your program on another machine.

**How to use `venv` (built into Python):**

1.  Go to your project directory in the terminal.
2.  Create a virtual environment: `python -m venv .venv` (instead of `.venv`, you can use any other name).
3.  Activate the environment:
    - Linux/macOS:
      ```bash
      source .venv/bin/activate
      ```
    - Windows (cmd):
      ```bat
      .venv\Scripts\activate.bat
      ```
    - Windows (PowerShell):
      ```powershell
      .venv\Scripts\Activate.ps1
      ```
4.  After activation, the terminal prompt will change to indicate the environment name. Now you can safely install packages: `pip install requests beautifulsoup4`
5.  To exit the environment: `deactivate`

Remember: using a virtual environment is almost always better than `sudo pip install`!

**Managing dependencies:**

To make it easier to reproduce the environment on another machine, you should save the used libraries into a `requirements.txt` file:  
`pip freeze > requirements.txt`.  
To install the required project libraries saved in `requirements.txt`:  
`pip install -r requirements.txt`.  
Remember to run these commands after activating the virtual environment.

## Additional notes

While solving the assignment, you will need to download pages from the internet and create your own markdown pages:

- [https://requests.readthedocs.io/en/latest/](https://requests.readthedocs.io/en/latest/)
- [https://aksakalli.github.io/jekyll-doc-theme/docs/cheatsheet/](https://aksakalli.github.io/jekyll-doc-theme/docs/cheatsheet/)

You may use any tools to scrape the web page, but you must write your own program to do so. Useful tools include:

- [BeautifulSoup](https://www.crummy.com/software/BeautifulSoup/)
- The [re](https://docs.python.org/3/library/re.html) module from Python’s standard library (warning: HTML is not a regular language, so regex can easily fail)

To manage the virtual environment, you can also use `uv` – a modern, very fast tool that replaces `pip` and `venv`. You need to install it first (e.g., `pipx install uv` or `pip install uv`).

1.  Go to your project directory.
2.  Create and activate the environment in one command: `uv venv`
3.  Install packages: `uv pip install requests beautifulsoup4`
4.  Save libraries: `uv pip freeze > requirements.txt`
5.  Install packages from file: `uv pip install -r requirements.txt`
