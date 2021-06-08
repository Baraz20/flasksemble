from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_bcrypt import Bcrypt
from flask_login import LoginManager
from flask_mail import Mail
import os
# init flask app
app = Flask(__name__.split('.')[0])

# how the secret key will be generated.
# >>> import secrets
# >>> secrets.token_hex(16)
# '25fd9***************************'
app.config['SECRET_KEY'] = '25fd958a42dfeaab834633a70c9624cf'
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///site.db'
db = SQLAlchemy(app)
bcrypt = Bcrypt(app)

# login and session manager
login_manager = LoginManager(app)
login_manager.login_view = 'login'
login_manager.login_message_category = 'info'

# Mailing service 
# TODO FIX THIS
app.config['MAIL_SERVER'] = 'smtp.googlemail.com'
app.config['MAIL_PORT'] = 587
app.config['MAIL_USE_TLS'] = True
# secure way to get email creds
app.config['MAIL_USERNAME'] = os.environ.get('EMAIL_USER') 
app.config['MAIL_PASSWORD'] = os.environ.get('EMAIL_PASSWORD') 
mail = Mail(app)

# assembler setup
app.config['ASSEMBLER_PATH'] = r'./assembler/'
app.config['ASSEMBLER_BIN'] = r'assembler.out'
app.config['ASSEMBLER_CMD'] = app.config['ASSEMBLER_PATH'] + app.config['ASSEMBLER_BIN']
app.config['ASSEMBLY_EXT'] = '.as'
app.config['OBJECT_EXT'] = '.ob'
app.config['EXTERNAL_EXT'] = '.ext'
app.config['ENTRY_EXT'] = '.ent'

# import this so routes work for this
from flasksemble import routes