#!/usr/bin/env python3
import smtplib, ssl, email
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.parser import Parser
from email.message import EmailMessage
import time
import threading
import random


port = 465  # For SSL
password = "bla"
sender_email = "my@gmail.com"
receiver_email = ["a@xvknp.com","b@xvknp.com","c@xvknp.com"]

message = EmailMessage();

with open('data/test.eml') as fp:
    message = email.message_from_file(fp)


# Create a secure SSL context

#/*, context=context*/
def send_email():    
    try:
        context = ssl.create_default_context()
        #context.load_verify_locations('../conf/RootCA.crt')
        context.check_hostname = False
        context.verify_mode = ssl.CERT_NONE
        server = smtplib.SMTP("localhost", 2525)
        server.set_debuglevel(1)
        server.ehlo() # Can be omitted
        server.starttls(context=context) # Secure the connection
    # server.ehlo() # Can be omitted
    # time.sleep(60*6)
        server.help()
        server.rset()
        server.verify("asd@asd.com")
        server.noop()    
        server.login(sender_email, password)    
        
        #server.sendmail(sender_email, receiver_email, "AAA")
        server.send_message(message)
        
        
    except Exception as e:
        # Print any error messages to stdout
        print(e)
    finally:
        server.quit() 

def send_email_ssl():    
    try:
        context = ssl.create_default_context()
        #context.load_verify_locations('../conf/RootCA.crt')        
        context.check_hostname = False
        context.verify_mode = ssl.CERT_NONE

        server = smtplib.SMTP_SSL("localhost", 4587, context=context)
        server.set_debuglevel(1)
        server.ehlo() # Can be omitted
    # server.starttls(context=context) # Secure the connection
    # server.ehlo() # Can be omitted
    # time.sleep(60*6)
        #dd = random.randrange(10)
        #if dd == 5:
        #    time.sleep(60*6)
            
        server.help()
        server.rset()
        server.verify("asd@asd.com")
        server.noop()    
        server.login(sender_email, password)    
        
        #server.sendmail(sender_email, receiver_email, "XCXX")
        server.send_message(message)
        
        
    except Exception as e:
        # Print any error messages to stdout
        print(e)
    finally:
        server.quit() 

threads = list()

for i in range(1):
    x = threading.Thread(target=send_email);
    threads.append(x)
    x.start()
    print("no")
    

#for i in range(10):
#    x = threading.Thread(target=send_email_ssl);
#    threads.append(x)
#    x.start()
#for t in threads:
#    t.join()
