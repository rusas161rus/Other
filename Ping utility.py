import os, platform
from urllib import response

host = "ya.ru"

response = os.system(
    "ping 87.250.250.242 " + ("-n 1" if 
    platform.system().lower()
    =="windows" else "-c 1") + host
)

print(not bool(response))
