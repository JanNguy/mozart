from collections import defaultdict
from pathlib import Path
from random import Random

import tensorflow as tf
from tensorflow import keras

from donnees import Categorie, X, Y

EPOCHS = 30
MODEL_PATH = Path("mozart.keras")
VALIDATION_RATIO = 0.2


def verifier_donnees() -> None:
    if not X:
        raise ValueError("X doit contenir au moins une requete.")
    if len(X) != len(Y):
        raise ValueError("X et Y doivent avoir la meme longueur.")
    if any(not isinstance(requete, str) or not requete.strip() for requete in X):
        raise ValueError("Chaque requete de X doit etre un texte non vide.")
    if any(categorie not in Categorie for categorie in Y):
        raise ValueError("Chaque valeur de Y doit etre une Categorie valide.")


def separer_donnees() -> tuple[tf.Tensor, tf.Tensor, tf.Tensor, tf.Tensor]:
    exemples_par_categorie: dict[Categorie, list[str]] = defaultdict(list)
    for requete, categorie in zip(X, Y):
        exemples_par_categorie[categorie].append(requete)

    aleatoire = Random(42)
    entrainement: list[tuple[str, Categorie]] = []
    validation: list[tuple[str, Categorie]] = []
    for categorie, exemples in exemples_par_categorie.items():
        aleatoire.shuffle(exemples)
        taille_validation = max(1, round(len(exemples) * VALIDATION_RATIO))
        validation.extend((requete, categorie) for requete in exemples[:taille_validation])
        entrainement.extend((requete, categorie) for requete in exemples[taille_validation:])

    aleatoire.shuffle(entrainement)
    aleatoire.shuffle(validation)

    def convertir(exemples: list[tuple[str, Categorie]]) -> tuple[tf.Tensor, tf.Tensor]:
        requetes = tf.constant([requete for requete, _ in exemples])[:, tf.newaxis]
        categories = tf.constant([int(categorie) for _, categorie in exemples])
        return requetes, categories

    requetes_entrainement, categories_entrainement = convertir(entrainement)
    requetes_validation, categories_validation = convertir(validation)
    return (
        requetes_entrainement,
        categories_entrainement,
        requetes_validation,
        categories_validation,
    )

def creer_modele(requetes_entrainement: tf.Tensor) -> keras.Sequential:
    vectorisation = keras.layers.TextVectorization(
        max_tokens=2_000,
        output_mode="tf_idf",
        ngrams=2,
    )
    vectorisation.adapt(requetes_entrainement)

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
    modele.compile(
        optimizer="adam",
        loss="sparse_categorical_crossentropy",
        metrics=["accuracy"],
    )
    return modele


def afficher_matrice_confusion(categories: tf.Tensor, probabilites: tf.Tensor) -> None:
    predictions = tf.argmax(probabilites, axis=1, output_type=tf.int32)
    matrice = tf.math.confusion_matrix(
        categories,
        predictions,
        num_classes=len(Categorie),
    ).numpy()
    noms_categories = [categorie.name for categorie in Categorie]
    largeur = max(len(nom) for nom in noms_categories)
    print("\nMatrice de confusion (lignes: reel, colonnes: predit)")
    print(" " * (largeur + 2) + " ".join(f"{nom:>{largeur}}" for nom in noms_categories))
    for nom, ligne in zip(noms_categories, matrice):
        print(f"{nom:>{largeur}}  " + " ".join(f"{valeur:>{largeur}}" for valeur in ligne))


def entrainer() -> keras.Sequential:
    verifier_donnees()
    tf.keras.utils.set_random_seed(42)

    (
        requetes_entrainement,
        categories_entrainement,
        requetes_validation,
        categories_validation,
    ) = separer_donnees()
    modele = creer_modele(requetes_entrainement)
    modele.fit(
        requetes_entrainement,
        categories_entrainement,
        validation_data=(requetes_validation, categories_validation),
        epochs=EPOCHS,
        callbacks=[
            keras.callbacks.EarlyStopping(
                monitor="val_loss",
                patience=5,
                min_delta=0.001,
                restore_best_weights=True,
            )
        ],
        verbose=2,
    )
    afficher_matrice_confusion(
        categories_validation,
        modele.predict(requetes_validation, verbose=0),
    )
    modele.save(MODEL_PATH)
    return modele


def predire(requete: str, modele: keras.Sequential) -> Categorie:
    probabilites = modele.predict(tf.constant([[requete]]), verbose=0)
    return Categorie(int(tf.argmax(probabilites[0])))


if __name__ == "__main__":
    modele = entrainer()
    exemple = "aide-moi a corriger ce bug"
    print(f"{exemple!r} -> {predire(exemple, modele).name}")
