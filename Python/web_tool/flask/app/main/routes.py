from app import db
from flask import render_template, flash, redirect, url_for, abort
from flask_login import current_user, login_user, logout_user, login_required
from app.auth.users import User
from flask import request as flask_request
from werkzeug.urls import url_parse
from urllib import request, parse, error
from app.main import bp
from app.errors.handlers import err_code_list

import json
import ssl


@bp.route('/')
@bp.route('/index')
def index():
    return render_template('main/index.html', title='Home')

@bp.route('/user')
@login_required
def user():
    return render_template('auth/user.html', title='User')

@bp.route('/tool')
def tool():
    return render_template('tool/tool.html', title='Tool')
