from flask import render_template
from app import db
from app.errors import bp

err_code_list = [{
    'code':404,
    'msg':'File Not Found'
    },
    {
    'code':500,
    'msg':'An unexpected error has occurred',
    'info':'The administrator has been notified. Sorry for the inconvenience!'
    }
]

@bp.app_errorhandler(404)
def not_found_error(error):
    return render_template('errors/error.html', title='Error', code_list=err_code_list, code=404), 404

@bp.app_errorhandler(500)
def internal_error(error):
    db.session.rollback()
    return render_template('errors/error.html', title='Error', code_list=err_code_list, code=500), 500