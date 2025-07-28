# Arquivo: train_nn.py (VERSÃO FINAL PARA KUHN POKER)

import pandas as pd
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense
import numpy as np

# --- Função de Engenharia de Features Definitiva ---
def state_to_features(card, history):
    """Cria um vetor numérico explícito e inequívoco para cada estado do jogo."""
    history = str(history) if pd.notna(history) else ""
    
    # One-hot encoding para a carta do jogador
    card_features = [0.0, 0.0, 0.0]
    card_features[card] = 1.0
    
    # One-hot encoding para o histórico de ações
    # Históricos possíveis: "", "p", "b", "pb"
    history_features = [0.0, 0.0, 0.0, 0.0]
    if history == "":
        history_features[0] = 1.0
    elif history == "p":
        history_features[1] = 1.0
    elif history == "b":
        history_features[2] = 1.0
    elif history == "pb":
        history_features[3] = 1.0
        
    # O vetor de features final é a combinação de tudo
    return card_features + history_features

# 1. Carregar os Dados
try:
    df = pd.read_csv("kuhn_poker_gto.csv")
    # Garante que não haja valores nulos no histórico
    df['history'] = df['history'].fillna('')
except FileNotFoundError:
    print("Erro: Arquivo 'kuhn_poker_gto.csv' nao encontrado.")
    exit()

# 2. Criar o Dataset com as Novas Features Explícitas
feature_list = df.apply(lambda row: state_to_features(row['card'], row['history']), axis=1)
features = np.array(feature_list.tolist())
labels = df[['pass_prob', 'bet_prob']].values

# 3. Construir um Modelo Simples e Eficaz
model = Sequential([
    # A camada de entrada agora tem 7 neurônios (3 para carta + 4 para histórico)
    Dense(16, activation='relu', input_shape=(7,)),
    Dense(16, activation='relu'),
    Dense(2, activation='softmax')
])

# 4. Compilar o Modelo
model.compile(optimizer='adam', loss='mean_squared_error')

print("\n--- INICIANDO TREINAMENTO FOCADO DA REDE NEURAL ---")
model.summary()

# 5. Treinar o Modelo com TODOS os dados e por mais tempo
history = model.fit(
    features, labels,
    epochs=500, # Mais épocas para garantir o aprendizado perfeito
    batch_size=4, # Lotes pequenos para um dataset pequeno
    verbose=0
)
print("Treinamento concluído.")

# 6. Avaliar a Perda Final
loss = model.evaluate(features, labels, verbose=0)
print(f"\nPerda (Loss) final no dataset completo: {loss:.8f}")
print("(Um valor muito próximo de zero significa que a rede aprendeu perfeitamente)")

# 7. Fazer Previsões
print("\n--- FAZENDO PREVISOES COM A REDE NEURAL DE ALTA PRECISAO ---")

# Criando os inputs para as 3 situações iniciais
input_J = np.array([state_to_features(0, '')])
input_Q = np.array([state_to_features(1, '')])
input_K = np.array([state_to_features(2, '')])

strategy_J = model.predict(input_J, verbose=0)[0]
strategy_Q = model.predict(input_Q, verbose=0)[0]
strategy_K = model.predict(input_K, verbose=0)[0]

print(f"\nEstrategia para Valete (J):")
print(f"  - PASS: {strategy_J[0]*100:.2f}%")
print(f"  - BET:  {strategy_J[1]*100:.2f}%")

print(f"\nEstrategia para Dama (Q):")
print(f"  - PASS: {strategy_Q[0]*100:.2f}%")
print(f"  - BET:  {strategy_Q[1]*100:.2f}%")

print(f"\nEstrategia para Rei (K):")
print(f"  - PASS: {strategy_K[0]*100:.2f}%")
print(f"  - BET:  {strategy_K[1]*100:.2f}%")

print("\nCompare estes resultados com a saida do seu programa C++ de 10M de iteracoes.")