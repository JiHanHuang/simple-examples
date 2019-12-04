from flask_wtf import FlaskForm
from wtforms import StringField, SubmitField, IntegerField
from wtforms.validators import DataRequired, URL
from app.auth.users import User
#import inspect

class InputStrForm(FlaskForm):
    input_str = StringField('Stirng', validators=[DataRequired()])
    submit = SubmitField('Submit')

class InputJsonForm(FlaskForm):
    input_json = StringField('Json', validators=[DataRequired()])
    addr = StringField('addr', validators=[DataRequired(), URL()])
    submit = SubmitField('Post Json')

class InputNumForm(FlaskForm):
    input_num = IntegerField('Number', validators=[DataRequired()])
    submit = SubmitField('Submit')