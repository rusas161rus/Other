import os
import psycopg2
from psycopg2 import pool


# Создание пула соединений
connection_pool = None

def create_connection_pool():
    global connection_pool
    connection_pool = psycopg2.pool.SimpleConnectionPool(
        minconn=1,
        maxconn=10,
        USER=os.getenv('USER'),                                
        PASSWORD=os.getenv('PASSWORD'),
        HOST=os.getenv('HOST'),
        PORT=os.getenv('PORT'),
        DATABASE=os.getenv('DATABASE'),
    )

def get_connection():
    global connection_pool
    if connection_pool is None:
        create_connection_pool()
    return connection_pool.getconn()

def release_connection(connection):
    global connection_pool
    connection_pool.putconn(connection)
