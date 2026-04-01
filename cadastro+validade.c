#include <stdio.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// Definições de cores ANSI para terminal
#define RED     "\x1b[31m"
#define YELLOW  "\x1b[33m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

#define MAX 100
#define LIMITE_CRITICO 3
#define LIMITE_ATENCAO 7

typedef struct {
    int codigo;
    char nome[50];
    char categoria[30];
    float peso;
    int quantidade;
    float preco;
    int dia, mes, ano;
    int dias_restantes;
    int prioridade;
} Alimento;

// Função para calcular dias restantes até a validade
int calcular_dias(int dia, int mes, int ano) {
    time_t agora = time(NULL);
    struct tm t = {0};
    t.tm_mday = dia;
    t.tm_mon = mes - 1;
    t.tm_year = ano - 1900;
    time_t validade = mktime(&t);
    double diff = difftime(validade, agora);
    return (int)(diff / 86400);
}

// Função para classificar a prioridade do alimento
void classificar(Alimento *a) {
    int dias = calcular_dias(a->dia, a->mes, a->ano);
    a->dias_restantes = dias;
    if(dias <= 0) a->prioridade = 0; // Vencido
    else if(dias <= LIMITE_CRITICO) a->prioridade = 1; // Crítico
    else if(dias <= LIMITE_ATENCAO) a->prioridade = 2; // Atenção
    else a->prioridade = 3; // OK
}

// Função para cadastrar um novo alimento
void cadastrar_alimento(Alimento estoque[], int *total) {
    if (*total >= MAX) {
        printf("\n[ERRO] Estoque cheio!\n");
        return;
    }

    Alimento novo;
    novo.codigo = *total + 1;

    printf("\n--- Cadastro de Alimento (ID: %d) ---\n", novo.codigo);
    printf("Nome: ");
    scanf(" %[^\n]", novo.nome);
    printf("Categoria: ");
    scanf(" %[^\n]", novo.categoria);
    printf("Peso (kg): ");
    scanf("%f", &novo.peso);
    while (getchar() != '\n'); // limpa o buffer
    printf("Quantidade em estoque: ");
    scanf("%d", &novo.quantidade);
    while (getchar() != '\n'); // limpa o buffer
    printf("Preco unitario (R$): ");
    scanf("%f", &novo.preco);
    while (getchar() != '\n'); // limpa o buffer
    printf("Data de validade (DD MM AAAA): ");
    scanf("%d %d %d", &novo.dia, &novo.mes, &novo.ano);
    while (getchar() != '\n'); // limpa o buffer

    classificar(&novo);
    estoque[*total] = novo;
    (*total)++;
    printf("\n[SUCESSO] Produto '%s' adicionado!\n", novo.nome);
}

// Função para listar todos os produtos com cores no terminal
void listar(Alimento estoque[], int total) {
    if (total == 0) {
        printf("\n[AVISO] Nenhum alimento cadastrado.\n");
        return;
    }

    char *status[] = {"VENCIDO", "CRÍTICO", "ATENÇÃO", "OK"};

    printf("\n%-4s %-15s %-12s %-10s %-12s %-15s %-10s %s\n",
           "ID", "Nome", "Categoria", "Qtd", "Preco(R$)", "Validade", "Status", "Dias");
    printf("--------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        Alimento a = estoque[i];
        const char *color;
        switch (a.prioridade) {
            case 0: color = RED; break;
            case 1: color = YELLOW; break;
            case 2: color = YELLOW; break;
            case 3: color = GREEN; break;
            default: color = RESET;
        }
        printf("%-4d %-15s %-12s %-10d %-12.2f %02d/%02d/%04d [%s%-7s" RESET "] %d dias\n",
               a.codigo, a.nome, a.categoria, a.quantidade, a.preco,
               a.dia, a.mes, a.ano, color, status[a.prioridade], a.dias_restantes);
    }
}

// Função para buscar um produto
void buscar_produto(Alimento estoque[], int total) {
    if (total == 0) {
        printf("\n[AVISO] O estoque está vazio! Cadastre algo primeiro.\n");
        return;
    }

    int escolha;
    printf("\n--- LOCALIZAR PRODUTO ---");
    printf("\n1. Buscar por CODIGO");
    printf("\n2. Buscar por NOME");
    printf("\nEscolha: ");
    scanf("%d", &escolha);
    while (getchar() != '\n'); // limpa o buffer

    if (escolha == 1) {
        int codigo_busca;
        printf("Digite o codigo: ");
        scanf("%d", &codigo_busca);
        while (getchar() != '\n'); // limpa o buffer
        for (int i = 0; i < total; i++) {
            if (estoque[i].codigo == codigo_busca) {
                printf("\n[ACHOU!] O produto '%s' (R$ %.2f) está na categoria: [%s]\n",
                       estoque[i].nome, estoque[i].preco, estoque[i].categoria);
                return;
            }
        }
    } else if (escolha == 2) {
        char nome_busca[50];
        printf("Digite o nome exato: ");
        scanf(" %[^\n]", nome_busca);
        for (int i = 0; i < total; i++) {
            if (strcmp(estoque[i].nome, nome_busca) == 0) {
                printf("\n[ACHOU!] O produto ID %d (R$ %.2f) está na categoria: [%s]\n",
                       estoque[i].codigo, estoque[i].preco, estoque[i].categoria);
                return;
            }
        }
    }
    printf("\n[ERRO] Produto não localizado no sistema.\n");
}

// Função para salvar em TXT
void salvar_estoque(Alimento estoque[], int total) {
    FILE *arquivo = fopen("estoque.txt", "w");
    if (arquivo == NULL) {
        printf("\n[Erro] Não foi possível abrir o arquivo.\n");
        return;
    }
    // Escreve o cabeçalho
    fprintf(arquivo, "%-4s %-15s %-12s %-10s %-12s %-15s %-10s %s\n",
            "ID", "Nome", "Categoria", "Qtd", "Preco(R$)", "Validade", "Status", "Dias");
    fprintf(arquivo, "--------------------------------------------------------------------------------------------\n");

    // Escreve cada alimento
    for (int i = 0; i < total; i++) {
        Alimento a = estoque[i];
        char *status[] = {"VENCIDO", "CRÍTICO", "ATENÇÃO", "OK"};
        fprintf(arquivo, "%-4d %-15s %-12s %-10d %-12.2f %02d/%02d/%04d %-10s %d\n",
                a.codigo, a.nome, a.categoria, a.quantidade, a.preco,
                a.dia, a.mes, a.ano, status[a.prioridade], a.dias_restantes);
    }
    fclose(arquivo);
    printf("\n[SUCESSO] Arquivo salvo com sucesso, salvo em 'estoque.txt'.\n");
}

// Função principal
int main() {
    setlocale(LC_ALL, "portuguese");
    Alimento lista_estoque[MAX];
    int total_produtos = 0;
    int opcao;

    do {
        printf("\n===== CONTROLE DE ESTOQUE MENU =====\n");
        printf("1. Cadastrar Produto\n");
        printf("2. Listar Todos os Produtos\n");
        printf("3. Localizar Produto (Busca)\n");
        printf("4. Salvar arquivo em .TXT\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);
        while (getchar() != '\n'); // limpa o buffer

        if (opcao == 1) cadastrar_alimento(lista_estoque, &total_produtos);
        else if (opcao == 2) listar(lista_estoque, total_produtos);
        else if (opcao == 3) buscar_produto(lista_estoque, total_produtos);
        else if (opcao == 4) salvar_estoque(lista_estoque, total_produtos);
        else if (opcao != 0) printf("\n[ERRO] Opcao invalida!\n");

    } while (opcao != 0);

    // Salvamento automático ao sair
    salvar_estoque(lista_estoque, total_produtos);
    printf("\nEncerrando o sistema FRESHGUARD. Você tem %d produtos no estoque.\n", total_produtos);

    return 0;
}
