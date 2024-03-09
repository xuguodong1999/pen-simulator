import json

if __name__ == '__main__':
    input_file = '/home/xgd/datasets2/im2latex/raw/im2latex_formulas.lst.json'
    output_file = '/mnt/d/source/repos/xgd-project/apps/az_data_cli/im2latex_formulas.lst.json'
    examples = []
    with open(input_file, 'r') as f:
        json_obj = json.load(f)
        json_obj['valid'].sort(key=len, reverse=True)
        for latex in json_obj['valid']:
            examples.append({
                'text_type': 'latex',
                'source_type': 'handwriting',
                'content': latex,
            })
            examples.append({
                'text_type': 'latex',
                'source_type': 'svg',
                'content': latex,
            })
    with open(output_file, 'w') as f:
        json.dump(examples, f, ensure_ascii=False)
