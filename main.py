import numpy as np
import pandas as pd
import tensorflow as tf
import PLSTM
import DataLoader
from features import g_num_action, batch_size, lr, n_layers

data_path = "/Users/airtanfin/Documents/Python/PLSTM/data/skeleton.csv"
model_path = "/Users/airtanfin/Documents/Python/PLSTM/model/"
preds_path = "/Users/airtanfin/Documents/Python/PLSTM/data/preds.csv"


def eval_f(sess, model, datas):
    mats, labels, views = datas
    feed_dict = {
        model.skel_input: mats,
        model.plstm_keep_prob: 1.00
    }

    return sess.run(model.action_distribution, feed_dict)


config = tf.ConfigProto()
model = PLSTM.PLSTM(batch_size=batch_size, lr=lr, n_layers=n_layers)
dat = pd.read_csv(data_path, sep=';').values
data_dict = {
    'id': 0,
    'mat': dat,
    'view': None,
    'action': None,
    'actor': None
}
data = np.array([data_dict])
data_size = data.shape[0]
dl = DataLoader.DataLoader(skel_train=None, skel_test=data, mode="test")
preds = np.zeros((data_size, g_num_action))

with tf.Session(config=config) as sess:
    sess.run(tf.global_variables_initializer())
    saver = tf.train.Saver()
    saver.restore(sess, model_path)
    anchor = 0

    for i in range(data_size):
        if anchor > data_size - batch_size:
            break

        datas = dl.get_test_sample(batch_size, anchor, is_rotate=True)
        preds[i, :] = eval_f(sess, model, datas)
        anchor += batch_size

action_names = ["Sitting down", "Standing up", "Reading", "Staggering", "Falling", "Walking"]
res = {}

for ind, name in enumerate(action_names):
    res[name] = preds[:, ind]

df = pd.DataFrame(res, columns=action_names)
df.to_csv(preds_path, sep=';', index=False)
