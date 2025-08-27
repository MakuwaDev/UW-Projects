import os
import requests
from bs4 import BeautifulSoup

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
    else:
        print(f'Failed to download the page for {char}')
        quit()

for char in characters:
    with open(f'characters/{char.replace(" ", "_")}.html', 'w') as file:
        file.write('<!DOCTYPE html>\n')
        file.write('<html lang="en-US">\n')
        file.write('  <head>\n')
        file.write('    <meta name="viewport" content="width=device-width, initial-scale=1">\n')
        file.write('    <meta charset="utf-8">\n')
        file.write(f'    <title>{char} - Marvel Rivals</title>\n')
        file.write('    <link rel="stylesheet" href="/marvelrivals/assets/css/output.css">\n')
        file.write('  </head>\n')
        file.write('  <body class="flex items-center justify-start flex-col">\n')
        file.write('    <div class="bg-gray-950 w-fit p-4 shadow-lg flex items-center justify-center flex-col space-y-4 rounded-3xl m-12">\n')
        file.write('      <div class="flex items-center flex-col">\n')
        file.write(f'       <h1>{char}</h1>\n')
        file.write(f'       <p><img src="../images/{char.replace(" ", "_")}.png" alt="{char} Nameplate"></p>\n')
        file.write('      </div>\n')
        file.write('\n')
        file.write('      <div class="flex flex-row gap-4">\n')
        file.write('        <div class="bg-slate-500 hover:bg-slate-600 flex-auto flex flex-col items-center rounded-lg border p-4">\n')
        file.write('          <h2 class="text-center">Description</h2>\n')
        file.write(f'           <p class="text-center">{descriptions[char]}</p>\n')
        file.write('        </div>\n')
        file.write('        <div class="bg-slate-500 hover:bg-slate-600 flex-none flex flex-col items-center rounded-lg border p-4">\n')
        file.write(f'         <h2>Rank 1 {char} Marvel Rivals</h2>')
        file.write('\n        <h3>Google Results:</h3>\n\n')

        for i in range(3):
            with open(f'searches/{char.replace(" ", "_")}{i}.txt', 'r') as search:
                file.write(f'          <h5>- {search.read()}</h5>\n')

        file.write('        </div>\n')
        file.write('      </div>\n')
        file.write('    </div>\n')
        file.write('    <div class="bg-gray-950 w-full flex flex-col items-center self-end mt-auto p-4">\n')
        file.write('      <h2><a href="/marvelrivals/characters.html">Back to characters</a></h2>\n')
        file.write('      <h2><a href="/marvelrivals/index.html">Back to index</a></h2>\n')
        file.write('    </div>\n')
        file.write('  </body>\n')
        file.write('</html>\n')
