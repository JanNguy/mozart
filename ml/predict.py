#!/usr/bin/env python3
"""CLI de prediction : charge mozart.keras et imprime l'int de la categorie."""
import sys
import zipfile
from pathlib import Path

import h5py
import tensorflow as tf
from tensorflow import keras

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from ml.donnees import Categorie

MODEL_PATH = Path(__file__).resolve().parent / "mozart.keras"


def _charger_modele() -> keras.Sequential:
    """Reconstruit le modele (contourne le bug de serialisation TextVectorization)."""
    with zipfile.ZipFile(str(MODEL_PATH)) as archive:
        vocab = archive.read(
            "assets/layers/text_vectorization/vocabulary.txt"
        ).decode().split("\n")
        if vocab and vocab[-1] == "":
            vocab = vocab[:-1]
        with archive.open("model.weights.h5") as poids:
            with h5py.File(poids, "r") as f:
                idf = f["layers/text_vectorization/vars/idf_weights"][:]

    vectorisation = keras.layers.TextVectorization(
        max_tokens=2000, output_mode="tf_idf", ngrams=2
    )
    vectorisation.set_vocabulary(vocab, idf_weights=idf)
    modele = keras.Sequential([
        keras.Input(shape=(1,), dtype=tf.string),
        vectorisation,
        keras.layers.Dense(
            64,
            activation="relu",
            kernel_regularizer=keras.regularizers.l2(1e-4),
        ),
        keras.layers.Dropout(0.4),
        keras.layers.Dense(len(Categorie), activation="softmax"),
    ])
    modele.load_weights(str(MODEL_PATH))
    return modele


def main() -> None:
    if len(sys.argv) < 2:
        print("usage: predict.py <requete>", file=sys.stderr)
        sys.exit(1)
    modele = _charger_modele()
    requete = sys.argv[1]
    probabilites = modele.predict(tf.constant([[requete]]), verbose=0)
    categorie = Categorie(int(tf.argmax(probabilites[0])))
    print(int(categorie))


if __name__ == "__main__":
    main()
