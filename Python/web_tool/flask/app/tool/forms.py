from flask_wtf import FlaskForm
from wtforms import StringField, SubmitField, IntegerField, TextAreaField, SelectField
from wtforms.validators import DataRequired, URL, ValidationError
from app.auth.users import User
from app.tool.service import data_type_list
import json
#import inspect

class InputStrForm(FlaskForm):
    input_str = StringField('Name', validators=[DataRequired()])
    submit = SubmitField('Del')

class PostDataForm(FlaskForm):
    data_type = SelectField('Data Type', choices=data_type_list)
    input_json = TextAreaField('Data', validators=[DataRequired()])
    addr = StringField('addr', validators=[DataRequired(), URL()])
    submit = SubmitField('Post Data')

class GetDataForm(FlaskForm):
    data_type = SelectField('Data Type', choices=data_type_list)
    addr = StringField('addr', validators=[DataRequired(), URL()])
    submit = SubmitField('Submit')

class InputNumForm(FlaskForm):
    input_num = IntegerField('Code', validators=[DataRequired()])
    submit = SubmitField('Send')

class InputDataForm(FlaskForm):
    name = StringField('name', validators=[DataRequired()])
    input_data = TextAreaField('data', validators=[DataRequired()])
    data_type = SelectField('Data Type', choices=data_type_list)
    submit = SubmitField('OK')

    def validate_input_data(self, username):
        print('xxxxx' + self.data_type.data)
        if self.data_type.data == data_type_list[1][0]:
            try:
                json.loads(self.input_data)
            except json.JSONDecodeError:
                raise ValidationError('Invalid data format.')