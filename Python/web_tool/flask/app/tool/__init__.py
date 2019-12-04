from flask import Blueprint

bp = Blueprint('tool', __name__)

from app.tool import routes