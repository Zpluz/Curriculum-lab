class Tokenizer(object):
    def __init__(self, words, max_len):
        self.words = words
        self.max_len = max_len

    def fmm_split(self, text):
        """
        正向最大匹配分词算法
        :param text: 待分词字符串
        :return: 分词结果，以list形式存放，每个元素为分出的词
        """
        reslist = []
        remaintxt = text
        while len(remaintxt):
            objstr = remaintxt[0: self.max_len]
            if objstr in self.words:
                reslist.append(objstr)
                remaintxt = remaintxt[self.max_len:]
            else:
                while len(objstr):
                    if len(objstr) == 1:
                        reslist.append(objstr)
                        remaintxt = remaintxt[len(objstr):]
                        break
                    objstr = objstr[:-1]
                    if objstr in self.words:
                        reslist.append(objstr)
                        remaintxt = remaintxt[len(objstr):]
                        break
        return reslist

    def rmm_split(self, text):
        """
        逆向最大匹配分词算法
        :param text: 待分词字符串
        :return: 分词结果，以list形式存放，每个元素为分出的词
        """
        reslist = []
        remaintxt = text
        while len(remaintxt):
            objstr = remaintxt[-self.max_len:]
            if objstr in self.words:
                reslist.append(objstr)
                remaintxt = remaintxt[:-self.max_len]
            else:
                while len(objstr):
                    if len(objstr) == 1:
                        reslist.append(objstr)
                        remaintxt = remaintxt[:-len(objstr)]
                        break
                    objstr = objstr[1:]
                    if objstr in self.words:
                        reslist.append(objstr)
                        remaintxt = remaintxt[:-len(objstr)]
                        break
        reslist.reverse()
        return reslist

    def bimm_split(self, text):
        """
        双向最大匹配分词算法
        :param text: 待分词字符串
        :return: 分词结果，以list形式存放，每个元素为分出的词
        """
        fmm_list = self.fmm_split(text)
        rmm_list = self.rmm_split(text)
        if len(fmm_list) > len(rmm_list):
            return rmm_list
        elif len(fmm_list) < len(rmm_list):
            return fmm_list
        elif fmm_list == rmm_list:
            return rmm_list
        elif fmm_list != rmm_list:
            fmm_single_count = sum(1 for i in fmm_list if len(i) == 1)
            rmm_single_count = sum(1 for i in rmm_list if len(i) == 1)
            if fmm_single_count < rmm_single_count:
                return fmm_list
            else:
                return rmm_list




def load_dict(path):
    tmp = set()
    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            word = line.strip().split(' ')[0]
            tmp.add(word)
    return tmp


if __name__ == '__main__':
    words = load_dict('dict.txt')
    max_len = max(map(len, [word for word in words]))

    # test
    tokenizer = Tokenizer(words, 3)
    texts = [
        '研究生命的起源',
        '无线电法国别研究',
        '人要是行，干一行行一行，一行行行行行，行行行干哪行都行。'
    ]
    for text in texts:
        # 前向最大匹配
        print('前向最大匹配:', '/'.join(tokenizer.fmm_split(text)))
        # 后向最大匹配
        print('后向最大匹配:', '/'.join(tokenizer.rmm_split(text)))
        # 双向最大匹配
        print('双向最大匹配:', '/'.join(tokenizer.bimm_split(text)))
        print('')
