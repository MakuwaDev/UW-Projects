import os
import requests
from bs4 import BeautifulSoup
import googlesearch

# Download the characters list
URL = 'https://www.radiotimes.com/technology/gaming/marvel-rivals-characters-list/'
characters = []
r = requests.get(URL)

if r.status_code == 200:
    soup = BeautifulSoup(r.text, 'html.parser')
    target_section = soup.find(lambda tag: tag and tag.text == 'Below are all the playable characters in Marvel Rivals at present:')
    if target_section:
        list = target_section.find_next('ul')

        if list:
            for item in list.find_all('li'):
                characters.append(item.text.strip())
        else:
            print('List not found')
            quit()
    else:
        print('Target section not found')
        quit()
else:
    print('Failed to download the page')
    quit()

characters = sorted(["Cloak & Dagger" if x == "Cloak and Dagger" else x for x in characters])

# Download character descriptions and nameplates
os.makedirs('images', exist_ok=True)
descriptions = {}
for char in characters:
    char_url = f'https://marvelrivals.fandom.com/wiki/{char.replace(" ", "_")}'
    r = requests.get(char_url)

    if r.status_code == 200:
        soup = BeautifulSoup(r.text, 'html.parser')

        description = soup.find('div', class_='pull-quote__text')
        if description:
            descriptions[char] = description.text.strip()
        else:
            print(f'Description not found for {char}')
            quit()

        nameplate_tag = soup.find('img', {'data-image-name': f'{char} Full Nameplate - {char}.png'})
        if nameplate_tag:
            nameplate_url = nameplate_tag['src']
            nameplate = requests.get(nameplate_url)
            if nameplate.status_code == 200:
                with open(f'images/{char.replace(" ", "_")}.png', 'wb') as f:
                    f.write(nameplate.content)
            else:
                print(f'Failed to download the nameplate for {char}')
                quit()
        else:
            print(f'Nameplate not found for {char}')
            quit()
    else:
        print(f'Failed to download the page for {char}')
        quit()

# Browse the web for each character
os.makedirs('searches', exist_ok=True)

for char in characters:
    if os.path.exists(f'searches/{char.replace(" ", "_")}0.txt'):
        print(f'Skipping {char}')
        continue

    search_results = googlesearch.search(f'Rank 1 {char} Marvel Rvials -site:reddit.com', stop=3)

    i = -1
    for result in search_results:
        i += 1
        r = requests.get(result)

        if r.status_code == 200:
            soup = BeautifulSoup(r.text, 'html.parser')
            title = soup.head.find('title')

            if title:
                with open(f'searches/{char.replace(" ", "_")}{i}.txt', 'w') as f:
                    f.write(title.text)
            else:
                print(f'Failed to get the title for {char} {i}')
                continue
        else:
            with open(f'searches/{char.replace(" ", "_")}{i}.txt', 'w') as f:
                f.write('Failed to download the page')

# Generate character specific pages
os.makedirs('characters', exist_ok=True)
for char in characters:
    with open(f'characters/{char.replace(" ", "_")}.md', 'w') as page:
        page.write('---\n')
        page.write('layout: default\n')
        page.write(f'title: {char}\n')
        page.write('---\n\n')

        page.write(f'# {char}\n\n')
        page.write(f'![{char} Nameplate](../images/{char.replace(" ", "_")}.png)\n\n')

        page.write('## Description\n\n')
        page.write(f'    {descriptions[char]}\n\n')

        page.write(f'## Rank 1 {char} Marvel Rivals\n\n')
        page.write('### Google Results:\n\n')

        for i in range(3):
            with open(f'searches/{char.replace(" ", "_")}{i}.txt', 'r') as search:
                page.write(f'##### - {search.read()}\n')
        
        page.write('\n## [Back to characters]({% link characters.md %})\n\n')
        page.write('## [Back to index]({{ site.baseurl }}/index.html)\n\n')

# Generate the characters page
with open('characters.md', 'w') as page:
    page.write('---\n')
    page.write('layout: default\n')
    page.write('title: Characters\n')
    page.write('---\n\n')

    page.write('# Marvel Rivals Characters\n\n')

    for char in characters:
        page.write(f'## - **{char}** - [some google searches](\u007b% link characters/{char.replace(" ", "_")}.md %\u007d)\n\n')
        page.write(f'   {descriptions[char]}\n\n')

    page.write('## [Back to index]({{ site.baseurl }}/index.html)\n\n')

# Generate the index page
with open('index.md', 'w') as page:
    page.write('---\n')
    page.write('layout: default\n')
    page.write('title: Index\n')
    page.write('---\n\n')

    page.write('# Marvel Rivals\n\n')
    page.write('### As I\'m super burnt out and have no idea what im doing,'
               ' I\'ll just post about something that constantly wastes my time '
               'and is the reason I\'m not performing well.\n\n')
    
    page.write('## Characters\n')
    page.write('### [link to characters lmao]({% link characters.md %})\n')