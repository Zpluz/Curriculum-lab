import torch
import torch.nn as nn
from torchcrf import CRF
from torch.nn.utils.rnn import pack_padded_sequence, pad_packed_sequence
from util.modeling_albert_bright import AlbertConfig, AlbertModel
from transformers import BertConfig, BertModel


class CWS(nn.Module):
    def __init__(self, tag2id, embedding_dim, hidden_dim, pretrain):
        super(CWS, self).__init__()
        self.embedding_dim = embedding_dim
        self.hidden_dim = hidden_dim
        self.tag2id = tag2id
        self.tagset_size = len(tag2id)
        self.pretrain = pretrain
        if self.pretrain == 'pretrain/roberta_large/':
            self.config = BertConfig.from_pretrained(self.pretrain)
            self.bert = BertModel.from_pretrained(self.pretrain, config=self.config)
        else:
            self.config = AlbertConfig.from_pretrained(self.pretrain)
            self.bert = AlbertModel.from_pretrained(self.pretrain, config=self.config)
        for p in self.bert.parameters():
            p.requires_grad = False

        self.lstm = nn.LSTM(self.embedding_dim, self.hidden_dim // 2, num_layers=1,
                            bidirectional=True, batch_first=True)
        self.hidden2tag = nn.Linear(hidden_dim, self.tagset_size)
        self.crf = CRF(4, batch_first=True)

    def init_hidden(self, batch_size, device):
        return (torch.randn(2, batch_size, self.hidden_dim // 2, device=device),
                torch.randn(2, batch_size, self.hidden_dim // 2, device=device))

    def _get_lstm_features(self, sentence, length):
        batch_size, seq_len = sentence.size(0), sentence.size(1)

        # idx->embedding
        if self.pretrain == 'pretrain/roberta_large/':
            embeds = self.bert(sentence).last_hidden_state
        else:
            embeds = self.bert(sentence)[0]

        embeds = pack_padded_sequence(embeds, length, batch_first=True)
        # LSTM forward
        self.hidden = self.init_hidden(batch_size, sentence.device)
        lstm_out, self.hidden = self.lstm(embeds, self.hidden)
        lstm_out, _ = pad_packed_sequence(lstm_out, batch_first=True)
        lstm_feats = self.hidden2tag(lstm_out)
        return lstm_feats

    def forward(self, sentence, tags, mask, length):
        emissions = self._get_lstm_features(sentence, length)
        loss = -self.crf(emissions, tags, mask, reduction='mean')
        return loss

    def infer(self, sentence, mask, length, *tags):
        emissions = self._get_lstm_features(sentence, length)
        predict = self.crf.decode(emissions, mask)
        if tags:
            loss = -self.crf(emissions, tags[0], mask, reduction='mean')
            return predict, loss
        else:
            return predict
