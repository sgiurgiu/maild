#!/usr/bin/env python
import smtplib, ssl, email
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.parser import Parser
from email.message import EmailMessage

port = 2525  # For SSL
password = "bla"
sender_email = "my@gmail.com"
receiver_email = "a@xvknp.com"

message = EmailMessage();

with open('data/linkedin.eml') as fp:
    message = email.message_from_file(fp);


# Create a secure SSL context
context = ssl.create_default_context()
#/*, context=context*/
try:
    server = smtplib.SMTP("localhost", port)
    server.ehlo() # Can be omitted
    #server.starttls(context=context) # Secure the connection
    #server.ehlo() # Can be omitted
    server.login(sender_email, password)    
    
    server.sendmail(sender_email, receiver_email, message.as_string())
    
except Exception as e:
    # Print any error messages to stdout
    print(e)
finally:
    server.quit() 
