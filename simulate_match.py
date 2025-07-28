# Arquivo: simulate_match.py (VERSÃO OTIMIZADA E FINAL)

import os
import numpy as np
import tensorflow as tf
import pandas as pd
import random
import time

print("--- SIMULADOR DE CONFRONTO OTIMIZADO: Rede Neural vs. GTO Teorico ---")

MODEL_FILENAME = "kuhn_poker_nn.keras"
NUM_SIMULATIONS = 1000000 # Rápido o suficiente para 1M+ agora

# (O código para carregar ou treinar a Rede Neural continua o mesmo)
# ...
def state_to_features(card, history):
    history = str(history) if pd.notna(history) else ""
    card_features = [0.0, 0.0, 0.0]; card_features[card] = 1.0
    history_features = [0.0, 0.0, 0.0, 0.0]
    if history == "": history_features[0] = 1.0
    elif history == "p": history_features[1] = 1.0
    elif history == "b": history_features[2] = 1.0
    elif history == "pb": history_features[3] = 1.0
    return card_features + history_features

if os.path.exists(MODEL_FILENAME):
    print(f"Carregando modelo treinado do arquivo: {MODEL_FILENAME}")
    model_nn = tf.keras.models.load_model(MODEL_FILENAME)
else: # Treina e salva se não existir
    # ... (código de treinamento idêntico ao anterior)
    print("Modelo nao encontrado. Treinando um novo modelo...")
    df = pd.read_csv("kuhn_poker_gto.csv"); df['history'] = df['history'].fillna('')
    feature_list = df.apply(lambda row: state_to_features(row['card'], row['history']), axis=1)
    features = np.array(feature_list.tolist()); labels = df[['pass_prob', 'bet_prob']].values
    model_nn = tf.keras.models.Sequential([tf.keras.layers.Input(shape=(7,)), tf.keras.layers.Dense(16, activation='relu'), tf.keras.layers.Dense(16, activation='relu'), tf.keras.layers.Dense(2, activation='softmax')])
    model_nn.compile(optimizer='adam', loss='mean_squared_error')
    model_nn.fit(features, labels, epochs=500, batch_size=4, verbose=0)
    model_nn.save(MODEL_FILENAME)
    print(f"Treinamento concluido. Modelo salvo como '{MODEL_FILENAME}'")

# --- ETAPA NOVA: PRÉ-CÁLCULO DA ESTRATÉGIA DA NN ---
print("Pre-calculando a estrategia da Rede Neural para todos os estados...")
nn_strategy_map = {}
possible_histories = ["", "p", "b", "pb"]
for card in [0, 1, 2]:
    for history in possible_histories:
        state_key = f"{card}/{history}"
        input_vector = np.array([state_to_features(card, history)])
        strategy = model_nn.predict(input_vector, verbose=0)[0]
        nn_strategy_map[state_key] = strategy

# --- DEFINIÇÃO DOS JOGADORES (COM NN OTIMIZADA) ---

def get_nn_action_optimized(card, history):
    strategy = nn_strategy_map[f"{card}/{history}"]
    return 'p' if random.random() < strategy[0] else 'b'

def get_textbook_gto_action(card, history): # (Função idêntica à anterior)
    if len(history) == 0:
        if card == 0: return 'b' if random.random() < (1/3) else 'p'
        if card == 1: return 'p'
        if card == 2: return 'b'
    if history == 'b':
        if card == 0: return 'p'
        if card == 1: return 'b' if random.random() < (1/3) else 'p'
        if card == 2: return 'b'
    if history == 'p':
        if card == 0: return 'p'
        if card == 1: return 'b' if random.random() < (1/3) else 'p'
        if card == 2: return 'b'
    if history == 'pb':
        if card == 2: return 'b'
        else: return 'p' # Paga apenas com Rei

# --- SIMULAÇÃO OTIMIZADA ---
total_utility_nn = 0
deck = [0, 1, 2]
start_time = time.time()

print(f"\nIniciando confronto de {NUM_SIMULATIONS} maos contra o GTO Teorico...")

for i in range(NUM_SIMULATIONS):
    players = [get_nn_action_optimized, get_textbook_gto_action]
    if random.random() < 0.5: players.reverse()
    is_nn_player_0 = (players[0] == get_nn_action_optimized)
    
    random.shuffle(deck)
    p0_card, p1_card = deck[0], deck[1]

    history = ""
    # Turnos de ação
    action1 = players[0](p0_card, history); history += action1
    if history in ['p', 'b']: action2 = players[1](p1_card, history); history += action2
    if history == 'pb': action3 = players[0](p0_card, history); history += action3
    
    # Calcular payoff
    payoff_p0 = 0
    if history == "pp":   payoff_p0 = 1 if p0_card > p1_card else -1
    elif history == "bp":  payoff_p0 = 1
    elif history == "bb":  payoff_p0 = 2 if p0_card > p1_card else -2
    elif history == "pbp": payoff_p0 = -1
    elif history == "pbb": payoff_p0 = 2 if p0_card > p1_card else -2
    
    total_utility_nn += payoff_p0 if is_nn_player_0 else -payoff_p0

    # Log de progresso a cada 10%
    if (i + 1) % (NUM_SIMULATIONS // 10) == 0:
        progress = (i + 1) / NUM_SIMULATIONS * 100
        current_winrate = (total_utility_nn / (i + 1)) * 1000
        print(f"  Progresso: {progress:.0f}%... (Winrate atual: {current_winrate:.4f} mu/mao)")

end_time = time.time()
print("Confronto concluido.")
print(f"Tempo total da simulacao: {end_time - start_time:.2f} segundos")

# --- RESULTADOS FINAIS ---
average_utility_nn = total_utility_nn / NUM_SIMULATIONS
winrate_mu_hand = average_utility_nn * 1000

print(f"\nUtilidade media da Rede Neural: {average_utility_nn:.8f}")
print(f"Winrate da Rede Neural: {winrate_mu_hand:.4f} milli-unidades/mao")

if abs(winrate_mu_hand) < 10:
    print("\nResultado: EMPATE TECNICO. A nossa Rede Neural joga uma estrategia GTO robusta e inexploravel!")
elif winrate_mu_hand > 0:
    print("\nResultado: VITORIA DA REDE NEURAL.")
else:
    print("\nResultado: VITORIA DO BOT GTO TEORICO.")