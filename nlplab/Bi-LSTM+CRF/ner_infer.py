import torch
import pickle
from transformers import BertTokenizer
from tqdm import tqdm
from torch.utils.data import DataLoader
from dataloader import Sentence
PRETRAIN_PATH = 'pretrain/roberta_large/'


def entity_split(x, y, id2tag, entities, cur):
    start, end = -1, -1
    cur_category = None
    for j in range(len(x)):
        if id2tag[y[j]][:2] == 'B-':
            start = cur + j
            cur_category = id2tag[y[j]][2:]
        elif id2tag[y[j]][:2] == 'I-' and start != -1 and cur_category is not None:
            if id2tag[y[j]][2:] == cur_category:
                continue
            else:
                start, end = -1, -1
                cur_category = None
        elif id2tag[y[j]][:2] == 'E-' and start != -1 and cur_category is not None:
            if id2tag[y[j]][2:] == cur_category:
                end = cur + j
                entities.add((start, end, cur_category))
            start, end = -1, -1
            cur_category = None
        elif id2tag[y[j]][:2] == 'S-':
            entities.add((cur + j, cur + j, id2tag[y[j]][2:]))
            start, end = -1, -1
            cur_category = None
        else:
            start, end = -1, -1
            cur_category = None


def inference():
    model = torch.load('save/ner/model_epoch29.pkl')

    with open('data/nersave.pkl', 'rb') as inp:
        tag2id = pickle.load(inp)
        id2tag = pickle.load(inp)
        x_train = pickle.load(inp)
        y_train = pickle.load(inp)
        x_eval = pickle.load(inp)
        y_eval = pickle.load(inp)
        x_test = pickle.load(inp)
        y_test = pickle.load(inp)

    test_data = DataLoader(
        dataset=Sentence(x_test, y_test),
        shuffle=False,
        batch_size=32,
        collate_fn=Sentence.collate_fn,
        drop_last=False,
        num_workers=6
    )

    entity_predict = set()
    entity_label = set()
    with torch.no_grad():
        model.eval()
        cur = 0
        test_losses = []
        for sentence, label, mask, length in tqdm(test_data):
            sentence = sentence.cuda()
            label = label.cuda()
            mask = mask.cuda()
            predict, loss = model.infer(sentence, mask, length, label)
            test_losses.append(loss)

            for i in range(len(length)):
                entity_split(sentence[i, :length[i]], predict[i], id2tag, entity_predict, cur)
                entity_split(sentence[i, :length[i]], label[i, :length[i]], id2tag, entity_label, cur)
                cur += length[i]

        test_loss = sum(test_losses) / len(test_losses)

        right_predict = [i for i in entity_predict if i in entity_label]
        precision = 0.0
        recall = 0.0
        fscore = 0.0
        if len(right_predict) != 0:
            precision = float(len(right_predict)) / len(entity_predict)
            recall = float(len(right_predict)) / len(entity_label)
            fscore = (2 * precision * recall) / (precision + recall)

        print("NER inference result:")
        print("loss: %f" % test_loss)
        print("precision: %f" % precision)
        print("recall: %f" % recall)
        print("f1-score: %f" % fscore)
        model.train()


if __name__ == '__main__':
    inference()

