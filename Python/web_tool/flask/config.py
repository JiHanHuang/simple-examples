import os
from dotenv import load_dotenv

#flask top dir
basedir = os.path.abspath(os.path.dirname(__file__))
#app env
load_dotenv(os.path.join(basedir, '.appenv'))

class Config(object):
    #using to https ?
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'you-will-never-guess'
    #for sql
    SQLALCHEMY_DATABASE_URI = os.environ.get('DATABASE_URL') or \
        'sqlite:///' + os.path.join(basedir, 'app.db')
    #send msg to app if sql data change
    SQLALCHEMY_TRACK_MODIFICATIONS = False