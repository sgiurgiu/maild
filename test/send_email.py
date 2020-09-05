#!/usr/bin/env python
import smtplib, ssl, email
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.parser import Parser
from email.message import EmailMessage

port = 4587  # For SSL
password = "bla"
sender_email = "my@gmail.com"
receiver_email = ["a@xvknp.com","b@xvknp.com","c@xvknp.com"]

message = EmailMessage();

with open('data/travel_spam.eml') as fp:
    message = email.message_from_file(fp);


# Create a secure SSL context
context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
context.load_verify_locations('../conf/RootCA.crt')

#/*, context=context*/
try:
    server = smtplib.SMTP("localhost", port)
    server.set_debuglevel(1)
    server.ehlo() # Can be omitted
    server.starttls(context=context) # Secure the connection
    server.ehlo() # Can be omitted
    server.help()
    server.rset()
    server.verify("asd@asd.com")
    server.noop()    
    server.login(sender_email, password)    
    
    #server.sendmail(sender_email, receiver_email, "ASDASD")
    server.send_message(message)
    
    
except Exception as e:
    # Print any error messages to stdout
    print(e)
finally:
    server.quit() 
