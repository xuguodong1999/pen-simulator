```bash
conda create -n ps python=3.11
conda activate ps
pip install -r requirements-dev.txt
python setup.py bdist_wheel
```
