import MySQLdb
import mysql.connector
from datetime import datetime
mydb = mysql.connector.connect(
  host="localhost",
  user="root",
  passwd="26121997",
  database="pro1"
)

def ghigiatri_nhietdo(msg):

	a=datetime.now()
	b=msg.payload
	mycursor = mydb.cursor()
	sql = "INSERT INTO dulieu1(thoigian, nhietdo) VALUES(%s,%s)"
	val = (a,b)
	mycursor.execute(sql, val)
	mydb.commit()
	print(mycursor.rowcount, "record inserted.")

def ghigiatri_doam(msg):

	a=datetime.now()
	b=msg.payload
	mycursor = mydb.cursor()
	sql = "INSERT INTO dulieu2(thoigian, doam) VALUES(%s,%s)"
	val = (a,b)
	mycursor.execute(sql, val)
	mydb.commit()
	print(mycursor.rowcount, "record inserted.")

def ghigiatri_gas(msg):

	a=datetime.now()
	b=msg.payload
	mycursor = mydb.cursor()
	sql = "INSERT INTO dulieu4(thoigian, gas) VALUES(%s,%s)"
	val = (a,b)
	mycursor.execute(sql, val)
	mydb.commit()
	print(mycursor.rowcount, "record inserted.")

def ghigiatri_smoke(msg):

	a=datetime.now()
	b=msg.payload
	mycursor = mydb.cursor()
	sql = "INSERT INTO dulieu5(thoigian, smoke) VALUES(%s,%s)"
	val = (a,b)
	mycursor.execute(sql, val)
	mydb.commit()
	print(mycursor.rowcount, "record inserted.")



def ghigiatri(msg):
	if msg.topic == "sensor/temperature":
		ghigiatri_nhietdo(msg)
	elif msg.topic == "sensor/humidity":
		ghigiatri_doam(msg)
        elif msg.topic == "sensor/gas":
		ghigiatri_gas(msg)
        elif msg.topic == "sensor/smoke":
		ghigiatri_smoke(msg)
        
