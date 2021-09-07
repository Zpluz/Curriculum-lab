import torch
import pickle
from transformers import BertTokenizer
from tqdm import tqdm
PRETRAIN_PATH = 'pretrain/roberta_large/'

if __name__ == '__main__':
    tokenizer = BertTokenizer.from_pretrained(PRETRAIN_PATH)
    model = torch.load('save/cws/model_rb916.pkl', map_location=torch.device('cpu'))
    output = open('save/cws/cws_result.txt', 'w', encoding='utf-8')

    with open('data/datasave.pkl', 'rb') as inp:
        tag2id = pickle.load(inp)
        id2tag = pickle.load(inp)

    with open('data/test_data.txt', 'r', encoding='utf-8') as f:
        with tqdm(total=1000) as pbar:
            for test in f:
                test = test.strip()

                x = torch.LongTensor(1, len(test) + 2)
                mask = torch.ones_like(x, dtype=torch.uint8)
                length = [len(test) + 2]
                x[0, 0] = 101
                for i in range(len(test)):
                    x[0, i + 1] = tokenizer.convert_tokens_to_ids(test[i])
                x[0, len(test) + 1] = 102

                predict = model.infer(x, mask, length)[0]
                for i in range(len(test)):
                    print(test[i], end='', file=output)
                    if id2tag[predict[i + 1]] in ['E', 'S']:
                        print(' ', end='', file=output)
                print(file=output)
                pbar.update(1)
