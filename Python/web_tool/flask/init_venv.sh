#!/bin/bash
path=$(cd "$(dirname "$0")";pwd)
venv_name='flask_venv'

cd ${path}
if [ ! -d "${path}/${venv_name}" ]; then
    echo "python venv creating..."
    python3 -m venv "${venv_name}"
fi
echo "Change venv..."
conda deactivate
source ${venv_name}/bin/activate
echo "Install dependent files..."
pip install -r requirements.txt
echo "Init venv finish."

echo "Please run: source ${venv_name}/bin/activate"
