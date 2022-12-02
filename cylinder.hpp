#ifndef CYLINDER_H_
#define CYLINDER_H_

#include <stdint.h>

// ---
// Inicialização da classe.
//
// O fluxo assumido é de criação dos cilindros como variáveis globais, por isso
// não pode ocorrer nada com efeitos-colaterais no construtor (ex. acoplamento
// de interrupção). Isso deve ocorrer no função setup, que cabe ao usuário
// chamar em momento oportuno antes de usar outras funcionalidades.

// Diâmetro dos cilindros de laminação.
constexpr uint32_t laminator_cylinder_diameter = 100 /* mm */;
// Diâmetro do cilindro de extração do papel.
constexpr uint32_t extractor_cylinder_diameter = 50 /* mm */;

class Cylinder {
public:
  // Construtor que só inicializa as variáveis-membro.
  Cylinder(uint8_t motor_pin, uint8_t encoder_pin, uint32_t diameter);
  // Destrutor que desacopla a interrupção do pino do seu encoder.
  ~Cylinder();

  // Faz ações de configuração que causam efeitos-colateriais.
  void setup();

  // ---
  // API primária, com as utilidades que o usuário pode chamar depois de
  // configurar.

  // Dita a velocidade que deve ser atingida pelo motor, em mm/s (velocidade do
  // papel). Pode ser lida e escrita publicamente.
  volatile uint16_t speed_setpoint = 0;

  // Calcula um valor proporcional para ser escrito no pino do motor, baseado na
  // velocidade atual (que é inferida a partir dos pulsos e tempo desde a última
  // leitura).
  //
  // A lógica é que menos potência deve ser fornecida quando a velocidade já é
  // próxima do requerido, e mais quando for menor. No caso da velocidade estar
  // maior, nada é aplicado.
  void run();
  // Pára o motor (deixa de aplicar potência), mas não limpa o armazenamento do
  // último cálculo.
  void stop();

  // Retorna o último nível de potência atribuído.
  uint8_t power() const;

  uint32_t pulses() const;

private:
  // ---
  // API privada e estado interno.

  // Pino do motor, que deve ser configurado por parâmetro do construtor.
  const uint8_t motor_pin;
  // Pino do encoder, que deve ser configurado por parâmetro do construtor.
  const uint8_t encoder_pin;

  // Diâmetro do cilindro, que deve ser configurado por parâmetro do construtor.
  const uint32_t diameter;
  // Contador de pulsos desde a última leitura. Deve ser limpo após ler.
  uint32_t pulse_count = 0;
  // Tempo (em milisegundos desde o início) em que foi calculada a velocidade
  // mais recente.
  uint64_t last_pulse_check = 0;

  // Potência atualmente escrita no pino PWM.
  uint8_t current_power = 0;

  // ---
  // Rotinas de interrupção e funções utilitárias para capturar o pulso do
  // encoder.

  // Auxiliar estático para permitir passagem a pcint_attach. Não podemos passar
  // Cylinder::count_pulse diretamente porque o ponteiro para função precisa
  // saber a que objeto pertence. Por isso essa camada de indireção; o objeto
  // que corresponde dono da função-membro é passado como argumento.
  static void handle_encoder(Cylinder *volatile self);
  // Incrementa o contador de pulsos por 1.
  void count_pulse();
};

#endif // CYLINDER_H_
