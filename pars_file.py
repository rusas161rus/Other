import re

filename="proxy.conf" 

pattern="server_name" #что меняем
prog = re.compile(pattern)
with open(filename,"r") as myfile:
    for line in myfile:
        if prog.match(line):
            print(line)
            with open ('proxy.conf', 'r') as f:
                old_data = f.read()
            new_data = old_data.replace(line, 'server_name norvision.norelectrics.ru;\n') #на что меняем  
            with open ('proxy.conf', 'w') as f:
                f.write(new_data) 
            

pattern="listen" #что меняем
prog = re.compile(pattern)
with open(filename,"r") as myfile:
    for line in myfile:
        if prog.match(line):
            print(line)
            with open ('proxy.conf', 'r') as f:
                old_data = f.read()
            new_data = old_data.replace(line, 'listen 81;\n')  #на что меняем
            with open ('proxy.conf', 'w') as f:
                f.write(new_data) 


                
                
def main():
    
    file = open("proxy.conf", "r")
    license = file.readlines()
    file.close

    for line in license:
        line = line.strip().upper()
        #print(line)
                 

main()
