import pickle
import logging
import argparse
import os
import torch
from torch.utils.data import DataLoader
from torch.optim import Adam
from ner_model import NER
from dataloader import Sentence
from torch.utils.tensorboard import SummaryWriter
from tqdm import tqdm

PRETRAIN_PATH = 'pretrain/'
PRETRAIN_MODEL = 'roberta_large/'
embed_dim_dict = {'albert_tiny/': 312, 'albert_large/': 1024, 'roberta_large/': 1024}

log_writer = SummaryWriter('runs/ner/' + PRETRAIN_MODEL)


def get_param():
    parser = argparse.ArgumentParser()
    parser.add_argument('--embedding_dim', type=int, default=embed_dim_dict[PRETRAIN_MODEL])
    parser.add_argument('--lr', type=float, default=3e-4)
    parser.add_argument('--max_epoch', type=int, default=30)
    parser.add_argument('--batch_size', type=int, default=32)
    parser.add_argument('--hidden_dim', type=int, default=200)

    parser.add_argument('--cuda', action='store_true', default=True)
    return parser.parse_args()


def set_logger():
    log_file = os.path.join('save/ner/', 'ner_log.txt')
    logging.basicConfig(
        format='%(asctime)s %(levelname)-8s %(message)s',
        level=logging.DEBUG,
        datefmt='%Y-%m%d %H:%M:%S',
        filename=log_file,
        filemode='w',
    )

    # console = logging.StreamHandler()
    # console.setLevel(logging.DEBUG)
    # formatter = logging.Formatter('%(asctime)s %(levelname)-8s %(message)s')
    # console.setFormatter(formatter)
    # logging.getLogger('').addHandler(console)


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


def main(args):
    use_cuda = args.cuda and torch.cuda.is_available()

    with open('data/nersave.pkl', 'rb') as inp:
        tag2id = pickle.load(inp)
        id2tag = pickle.load(inp)
        x_train = pickle.load(inp)
        y_train = pickle.load(inp)
        x_eval = pickle.load(inp)
        y_eval = pickle.load(inp)

    # model = NER(tag2id, args.embedding_dim, args.hidden_dim, PRETRAIN_PATH + PRETRAIN_MODEL)
    model = torch.load('save/ner/model_epoch29.pkl')

    if use_cuda:
        model = model.cuda()
    for name, param in model.named_parameters():
        logging.debug('%s: %s, require_grad=%s' % (name, str(param.shape), str(param.requires_grad)))

    optimizer = Adam(model.parameters(), lr=args.lr)

    train_data = DataLoader(
        dataset=Sentence(x_train, y_train),
        shuffle=True,
        batch_size=args.batch_size,
        collate_fn=Sentence.collate_fn,
        drop_last=False,
        num_workers=6
    )

    eval_data = DataLoader(
        dataset=Sentence(x_eval, y_eval),
        shuffle=False,
        batch_size=args.batch_size,
        collate_fn=Sentence.collate_fn,
        drop_last=False,
        num_workers=6
    )

    # for sentence, label, mask, length in tqdm(train_data):
    #     if use_cuda:
    #         sentence = sentence.cuda()
    #         label = label.cuda()
    #         mask = mask.cuda()
    #     model.eval()
    #     log_writer.add_graph(model, [sentence, label, mask, torch.Tensor(length)], verbose=True)

    iteration = 0
    for epoch in range(args.max_epoch):
        log = []
        for sentence, label, mask, length in tqdm(train_data):
            if use_cuda:
                sentence = sentence.cuda()
                label = label.cuda()
                mask = mask.cuda()

            # forward
            loss = model(sentence, label, mask, length)
            log.append(loss.item())

            # backward
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

            log_writer.add_scalar('Loss/train', float(loss), iteration)
            iteration = iteration + 1
            if iteration % 100 == 0:
                logging.debug('epoch %d-step %d loss: %f' % (epoch, iteration, sum(log)/len(log)))
                log = []

        # test
        entity_predict = set()
        entity_label = set()
        with torch.no_grad():
            model.eval()
            cur = 0
            eval_losses = []
            for sentence, label, mask, length in tqdm(eval_data):
                if use_cuda:
                    sentence = sentence.cuda()
                    label = label.cuda()
                    mask = mask.cuda()
                predict, loss = model.infer(sentence, mask, length, label)
                eval_losses.append(loss)

                for i in range(len(length)):
                    entity_split(sentence[i, :length[i]], predict[i], id2tag, entity_predict, cur)
                    entity_split(sentence[i, :length[i]], label[i, :length[i]], id2tag, entity_label, cur)
                    cur += length[i]

            eval_loss = sum(eval_losses) / len(eval_losses)
            log_writer.add_scalar('Loss/eval', float(eval_loss), epoch)
            right_predict = [i for i in entity_predict if i in entity_label]

            precision = 0.0
            recall = 0.0
            fscore = 0.0
            if len(right_predict) != 0:
                precision = float(len(right_predict)) / len(entity_predict)
                recall = float(len(right_predict)) / len(entity_label)
                fscore = (2 * precision * recall) / (precision + recall)
            logging.info("precision: %f" % precision)
            logging.info("recall: %f" % recall)
            logging.info("f1-score: %f" % fscore)
            log_writer.add_scalars('Metrics', {'precision': precision, 'recall': recall, 'fscore': fscore}, epoch)
            print("precision: %f" % precision)
            print("recall: %f" % recall)
            print("f1-score: %f" % fscore)
            model.train()

        path_name = "save/ner/model_epoch" + str(epoch) + ".pkl"
        torch.save(model, path_name)
        logging.info("model has been saved in  %s" % path_name)


if __name__ == '__main__':
    set_logger()
    main(get_param())
