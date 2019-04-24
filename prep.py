import fileinput
import os

search=['current: ',' delta: ']
replace=['',',']
prepend=''

directory='Y:\\y\\sync\\current\\work\\biohacking\\r&d\\nerveStimulator\\arduino\\'

for root, dirs, files in os.walk(directory):
    print(root)
    
    for file in files:
        
        if file.endswith('.txt'):
            print(file)
            filename = os.path.join(root, file)
            #filename = filename.replace('/','\\', end='')
            if 'Periods' in file:
                prepend='currentTime,delta'
                print(filename)
                text_to_search='current: '
                replacement_text = ''
                
                
                with open(filename, "r+") as f: s = f.read(); f.seek(0); f.write(prepend+"\n" + s)
                
                for i in range(0,len(search)):
                    with fileinput.FileInput(filename, inplace=True, backup='.bak') as file: 
                        for line in file:    
                            print(line.replace(search[i], replace[i]), end='')
            else: 
                if 'Times' in file:
                    prepend='Times'
                    print(filename)
                    with open(filename, "r+") as f: s = f.read(); f.seek(0); f.write(prepend+"\n" + s)