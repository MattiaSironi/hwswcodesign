%% Set the parameters
num_days = 365*5;              % Number of days to simulate (1 year)
samples_per_day = 144;       % Number of samples per day (1 sample every 10 minutes)
total_samples = num_days * samples_per_day;

% Diurnal (daily) temperature variation
daily_temperature_range = 10;  % Change this to set the daily temperature range

% Seasonal temperature variation
seasonal_temperature_range = 5; % Change this to set the seasonal temperature range

% Create a time vector (1 sample every 10 minutes)
time_vector = linspace(0, num_days, total_samples);

% Initialize the temperature vector
temperature_data = zeros(1, total_samples);

%%Generate synthetic temperature data
for i = 1:total_samples
    day_of_year = mod(time_vector(i), num_days) + 1;  % Day of the year (1 to 365)
    time_of_day = mod(time_vector(i) * 24, 24);        % Time of day (0 to 24 hours)
    
    % Simulate diurnal temperature variation
    daily_variation = daily_temperature_range * sin(2 * pi * time_of_day / 24);
    
    % Simulate seasonal temperature variation
    seasonal_variation = seasonal_temperature_range * sin(0.001*2 * pi * day_of_year / num_days);
    
    temperature_data(i) = 25 + daily_variation + seasonal_variation + randn * 2; % 25°C base, random noise
    
end

% Plot the synthetic data
plot(time_vector, temperature_data);
xlabel('Time (days)');
ylabel('Temperature (°C)');
title('Synthetic Temperature Data for Plant');

% Save the data to a file
% save('synthetic_temperature_data.mat', 'temperature_data', 'time_vector');

% Load your synthetic temperature data or your real data

%load('synthetic_temperature_data.mat');  % Replace with your data

N=length(temperature_data);

train_set=temperature_data(1:N*0.8); 
test_set=temperature_data(N*0.8+1:end); 

%%.........................................................................
% % Load your synthetic temperature data or your real data
% %load('synthetic_temperature_data.mat');  % Replace with your data
% 
% % Initialize variables to store the best model and corresponding criteria
% best_model = [];
% best_aic = Inf;
% best_bic = Inf;
% 
% % Set maximum orders for the search
% max_p = 3; % Maximum nonseasonal autoregressive order
% max_d = 1; % Maximum differencing order
% max_q = 3; % Maximum nonseasonal moving average order
% max_P = 2; % Maximum seasonal autoregressive order
% max_D = 1; % Maximum seasonal differencing order
% max_Q = 2; % Maximum seasonal moving average order
% s = samples_per_day; % Seasonal period (samples per day)
% 
% % Iterate through different model orders and fit SARIMA models
% for p = 0:max_p
%     for d = 0:max_d
%         for q = 0:max_q
%             for P = 0:max_P
%                 for D = 0:max_D
%                     for Q = 0:max_Q
%                         try
%                             % Create and fit the SARIMA model
%                             sarima_model = arima('AR', p, 'D', d, 'MA', q, 'Seasonality', s, 'SAR', P, 'SMA', Q);
%                             sarima_model = estimate(sarima_model, train_set');
% 
%                             % Calculate AIC and BIC
%                             aic = aic(sarima_model);
%                             bic = bic(sarima_model);
% 
%                             % Update the best model if the criteria are lower
%                             if aic < best_aic && bic < best_bic
%                                 best_model = sarima_model;
%                                 best_aic = aic;
%                                 best_bic = bic;
%                             end
%                         catch
%                             % Handle model estimation errors
%                             continue;
%                         end
%                     end
%                 end
%             end
%         end
%     end
% end
% 
% % Display the best model orders and criteria
% fprintf('Best Model Orders (p, d, q, P, D, Q): %d, %d, %d, %d, %d, %d\n', best_model.AR, best_model.D, best_model.MA, best_model.SeasonalAR, best_model.SeasonalD, best_model.SeasonalMA);
% 
% fprintf('Best AIC: %f\n', best_aic);
% fprintf('Best BIC: %f\n', best_bic);




%%.........................................................................


%% Define the SARIMA model parameters
p = 0; % Autoregressive order
d = 1; % Differencing order
q = 0; % Moving average order
P = 0; % Seasonal autoregressive order
D = 1; % Seasonal differencing order
Q = 0; % Seasonal moving average order
s = samples_per_day; % Seasonal period (samples per day)

% Create and fit the SARIMA model
sarima_model = arima('AR', p, 'D', d, 'MA', q, 'Seasonality', s, 'SAR', P, 'SMA', Q);

sarima_model = estimate(sarima_model, train_set');

N_validation_test=floor(0.01*length(test_set)); 
% Forecast future values
forecast_horizon = N_validation_test;  % Number of forecasted samples (e.g., one day)
[forecast1, forecast_var] = forecast(sarima_model, forecast_horizon, 'Y0', test_set(1:0.99*length(test_set))');

figure

% Plot the original data and the forecasted data
t_test=time_vector(1:N_validation_test);
figure;
plot(t_test, test_set(0.99*length(test_set)+1:end), 'b', 'LineWidth', 1.5);
hold on;
%forecast_time_vector = linspace(max(t_test), max(t_test) + forecast_horizon / samples_per_day, forecast_horizon);
plot(t_test, forecast1, 'r', 'LineWidth', 1.5);


xlabel('Time (days)');
ylabel('Temperature (°C)');
title('SARIMA Forecasting');
legend('Original Data', 'Forecasted Data');
xlim([0 0.5]); 
ylim([0 50]); 

% Save the SARIMA model for future use
save('forecast.mat','forecast1'); 
save('test.mat','test_set'); 

save('sarima_model.mat', 'sarima_model');