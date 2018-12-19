import numpy as np
import pandas as pd
import tensorflow as tf
import PLSTM
import DataLoader
from features import batch_size, lr, n_layers

data_path = "/Users/airtanfin/Documents/Python/PLSTM/data/skeleton.csv"
model_path = "/Users/airtanfin/Documents/Python/PLSTM/model/"
preds_path = "/Users/airtanfin/Documents/Python/PLSTM/data/preds.csv"
n = 300
p = 10
action_names = ["Sitting down", "Standing up", "Reading", "Staggering", "Falling", "Walking"]


def eval_f(sess, model, data):
    feed_dict = {model.skel_input: data, model.plstm_keep_prob: 1.00}

    return sess.run(model.action_distribution, feed_dict)


a = 0
b = n

while pd.read_csv(data_path, header=None, sep=';').shape[0] < b:
    pass

model = PLSTM.PLSTM(batch_size=batch_size, lr=lr, n_layers=n_layers)
dat = np.array([pd.read_csv(data_path, header=None, sep=';', skiprows=a, nrows=b).values])
dl = DataLoader.DataLoader(skel_train=None, skel_test=dat, mode="test")

config = tf.ConfigProto()
sess = tf.Session(config=config)
sess.run(tf.global_variables_initializer())
saver = tf.train.Saver()
saver.restore(sess, model_path)

data = dl.get_test_sample(batch_size, 0, is_rotate=True)
preds = eval_f(sess, model, data)
df = pd.DataFrame(data=preds, columns=action_names)
df.to_csv(preds_path, sep=';', index=False)

while True:
    a += p
    b += p

    while pd.read_csv(data_path, header=None, sep=';').shape[0] < b:
        pass

    dat = np.array([pd.read_csv(data_path, header=None, sep=';', skiprows=a, nrows=b).values])
    dl = DataLoader.DataLoader(skel_train=None, skel_test=dat, mode="test")

    data = dl.get_test_sample(batch_size, 0, is_rotate=True)
    preds = eval_f(sess, model, data)
    df = df.append(pd.DataFrame(data=preds, columns=action_names))
    df.to_csv(preds_path, sep=';', index=False)

sess.close()
