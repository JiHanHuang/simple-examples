from app import db
from flask import render_template, flash, redirect, url_for, abort
from app.tool.forms import PostDataForm, InputNumForm, GetDataForm, InputDataForm, InputStrForm
from flask_login import current_user, login_user, logout_user, login_required
from app.auth.users import User
from flask import request as flask_request
from werkzeug.urls import url_parse
from urllib import request, parse, error
from app.tool import bp
from app.errors.handlers import err_code_list
from app.tool.service import Service, data_type_list

import json

@bp.route('/service', methods=['GET', 'POST'])
def tool_service():
    form = InputDataForm()
    if form.validate_on_submit():
        service = Service()
        service.name = form.name.data
        service.data_type = form.data_type.data
        service.data = form.input_data.data
        db.session.add(service)
        db.session.commit()
        flash(form.name.data + ' Save success!')
    return render_template('tool/service.html', title='Tool', form=form)

@bp.route('/service_list', methods=['GET', 'POST'])
def tool_service_list():
    form = InputStrForm()
    services = Service.query.all()
    if form.validate_on_submit():
        service = Service.query.filter_by(name=form.input_str.data).first_or_404()
        
        db.session.delete(service)
        db.session.commit()
        services = Service.query.all()
        flash(form.input_str.data + ' Del success!')
    return render_template('tool/service_list.html', title='Tool', form=form, services=services)

@bp.route('/service/<name>')
def tool_service_one(name):
    service = Service.query.filter_by(name=name).first_or_404()
    if service.data_type == data_type_list[1][0]:
        return json.loads(service.data)
    else:
        return service.data

@bp.route('/post_data', methods=['GET', 'POST'])
def tool_post_data():
    form_post = PostDataForm()
    response = ''
    if form_post.validate_on_submit():
        post_data = json.dumps(form_post.input_json.data)
        post_addr = form_post.addr.data
        post_type = form_post.data_type.data
        if post_type == data_type_list[1][0]:
            headers = {
                'Content-Type': 'application/json',
                'Accept': 'application/json'
            }
            data = json.loads(post_data)
        else:
            headers = ''
            data = post_data
        req = request.Request(post_addr, '', headers)
        try:
            f = request.urlopen(req, data=data.encode('utf-8'))
            response = f.read().decode('utf-8')
        except error.HTTPError as e:
            response = post_addr + str(e)
    return render_template('tool/post_data.html', title='Tool', form_post=form_post, response=response)

@bp.route('/get_data', methods=['GET', 'POST'])
def tool_get_data():
    form_get = GetDataForm()
    response = ''
    if form_get.validate_on_submit():
        post_addr = form_get.addr.data
        req = request.Request(post_addr)
        try:
            f = request.urlopen(req)
            if form_get.data_type == data_type_list[1][0]:
                res_data = json.loads(f.read())
                response = json.dumps(res_data,sort_keys=True, indent=4)
            else:
                response = f.read()
        except error.HTTPError as e:
            response = post_addr + str(e)
    return render_template('tool/get_data.html', title='Tool', form_get=form_get, response=response)

@bp.route('/export_err', methods=['GET', 'POST'])
def tool_export_err():
    form = InputNumForm()
    if form.validate_on_submit():
        err_code = form.input_num.data
        code_list = []
        for code in err_code_list:
            code_list.append(code['code'])
        if err_code in code_list:
            abort(err_code)
        else:
            flash('Invalid error code.', err_code)
    return render_template('tool/export_err.html', title='Export ERR', form_err=form)


@bp.route('/export_err/<err_code>')
def tool_export_err_code(err_code):
    code_list = []
    err_code = int(err_code)
    for code in err_code_list:
        code_list.append(code['code'])
    if err_code in code_list:
        abort(err_code)
    else:
        abort(404)